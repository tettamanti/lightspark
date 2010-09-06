/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009,2010  Alessandro Pignotti (a.pignotti@sssup.it)
    Copyright (C) 2010  Luca Tettamanti (kronos.it@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "alsaplugin.h"

using lightspark::AudioDecoder;

int AlsaPlugin::next_stream_id = 1;
const char *AlsaPlugin::sound_device = "default";
const int AlsaPlugin::sample_size = snd_pcm_format_width(sample_format) / 8;

void AlsaPlugin::set_device(std::string desiredDevice,
		IAudioPlugin::DEVICE_TYPES desiredType)
{
	/* not yet implemented */
}

AudioStream* AlsaPlugin::createStream(AudioDecoder* decoder)
{
	int err, dir;

	AlsaAudioStream *stream = new AlsaAudioStream(next_stream_id++, decoder);

	err = snd_pcm_open(&stream->handle, sound_device, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		LOG(LOG_ERROR, "Failed to open ALSA device " << sound_device
				<< ": " << snd_strerror(err));
		goto out_err;
	}

	/* Setup required format */
	err = snd_pcm_set_params(stream->handle,
			sample_format,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			decoder->channelCount,
			decoder->sampleRate,
			1,
			500000 /* 0.5 sec */);
	if (err < 0) {
		LOG(LOG_ERROR, "Failed to setup ALSA stream: "
				<< snd_strerror(err));
		goto out_err;
	}
	
	/* Read back period */
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_hw_params_alloca(&hwparams);
	
	err = snd_pcm_hw_params_current(stream->handle, hwparams);
	if (err < 0) {
		LOG(LOG_ERROR, "Failed to read hw params: "
				<< snd_strerror(err));
		goto out_err;
	}
	
	err = snd_pcm_hw_params_get_period_size(hwparams, &stream->period_size, &dir);
	if (err < 0) {
		LOG(LOG_ERROR, "Failed to read period size: "
				<< snd_strerror(err));
		goto out_err;
	}

	LOG(LOG_TRACE, "Stream " << stream->id << endl
			<< decoder->sampleRate << "Hz" << endl
			<< decoder->channelCount << " channels" << endl
			<< "period size " << stream->period_size);

	/* Enable async callback */
	err = snd_async_add_pcm_handler(&stream->async_handler, stream->handle,
			async_callback, stream);
	if (err < 0) {
		LOG(LOG_ERROR, "Failed to setup async handler: "
				<< snd_strerror(err));
		goto out_err;
	}

	streams.push_back(stream);

	/* Kick off playback */
	stream->fill();
	stream->start();

	return stream;
out_err:
	delete stream;
	return NULL;
}

void AlsaPlugin::freeStream(AudioStream *stream)
{
	assert(stream);

	AlsaAudioStream *s = static_cast<AlsaAudioStream *>(stream);
	streams.remove(s);

	delete stream;
}

void AlsaAudioStream::fill()
{
	snd_pcm_uframes_t avail = snd_pcm_avail_update(handle);

	uint32_t buf_size = period_size * AlsaPlugin::sample_size * decoder->channelCount;
	uint8_t *buf = new uint8_t[buf_size];
/*
	if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN ||
			snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED) {
		throw std::runtime_error("invalid state");
	}
*/
	
	/* Fill the buffer using the decoder */
	while (avail > period_size) {
		uint32_t ret = decoder->copyFrame((int16_t *)buf, buf_size);
		if (!ret)
			/* No more data */
			break;

		snd_pcm_uframes_t frames = snd_pcm_bytes_to_frames(handle, ret);
		snd_pcm_sframes_t written;

		written = snd_pcm_writei(handle, buf, frames);
		if (written < 0) {
			int err;
        		LOG(LOG_TRACE, "stream recovery[" << id << "]: " << written);
			err = snd_pcm_recover(handle, written, 0);
			if (err < 0)
				/* Recovery failed, now what? */
				LOG(LOG_ERROR, "Audio write error: "
						<< snd_strerror(err));
		} else {
			frames_played += written;
		}
		avail = snd_pcm_avail_update(handle);
	}
	
	delete[] buf;
}

void AlsaPlugin::async_callback(snd_async_handler_t *ahandler)
{
	void *data = snd_async_handler_get_callback_private(ahandler);

	AlsaAudioStream *stream = static_cast<AlsaAudioStream *>(data);
	if (!stream->decoder->hasDecodedFrames())
		/* No decoded data available yet, delay upload */
		return;

	stream->fill();
}

AlsaPlugin::~AlsaPlugin()
{
	/* Kill all the streams */
	for (stream_iterator it = streams.begin(); it != streams.end(); ++it) {
		(*it)->stop();
		delete *it;
	}
}


bool AlsaPlugin::isTimingAvailable() const
{
	/* Uh, I guess so */
	return true;
}

uint32_t AlsaAudioStream::getPlayedTime()
{
	uint32_t ret;
#if 1
	/* TODO: compensate for buffering */
	ret = frames_played * 1000 / decoder->sampleRate;
#else
	struct timeval now;
	gettimeofday(&now, NULL);

	ret = (now.tv_sec * 1000 + now.tv_usec / 1000) -
		(stream->start_time.tv_sec * 1000 + stream->start_time.tv_usec / 1000);
#endif
	LOG(LOG_TRACE, "SoundManager::getPlayedTime[" << id << "]: " << ret);
	return ret;
}

void AlsaAudioStream::start() {
	snd_pcm_start(handle);
}

void AlsaAudioStream::stop() {
	/* XXX: drain? */
	snd_pcm_drop(handle);
}

AlsaAudioStream::~AlsaAudioStream() {
	snd_pcm_close(handle);
}

extern "C" DLL_PUBLIC IPlugin *create()
{
	return new AlsaPlugin();
}

extern "C" DLL_PUBLIC void release(IPlugin *plugin)
{
	delete plugin;
}
