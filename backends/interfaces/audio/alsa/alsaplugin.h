/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009,2010  Alessandro Pignotti (a.pignotti@sssup.it)
    Copyright (C) 2010 Luca Tettamanti (kronos.it@gmail.com)

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

#ifndef PULSEPLUGIN_H
#define PULSEPLUGIN_H

#include <alsa/asoundlib.h>
#include "../IAudioPlugin.h"
#include "../../../decoder.h"
#include "../../../../compat.h"

using lightspark::AudioDecoder;

class AlsaAudioStream;

class AlsaPlugin : public IAudioPlugin
{
private:
	static const char *sound_device;

	static int next_stream_id;

	std::list<AlsaAudioStream *> streams;
	typedef std::list<AlsaAudioStream *>::iterator stream_iterator;

	static void async_callback(snd_async_handler_t *ahandler);
public:
	static const snd_pcm_format_t sample_format = SND_PCM_FORMAT_S16_LE;
	static const int sample_size;

	AlsaPlugin(): IAudioPlugin("ALSA plugin", "alsa", false) { }

	void set_device(std::string desiredDevice, IAudioPlugin::DEVICE_TYPES desiredType);

	AudioStream *createStream(AudioDecoder *decoder);
	void freeStream(AudioStream *audioStream);

	bool isTimingAvailable() const;
	~AlsaPlugin();
};

struct AlsaAudioStream: public AudioStream
{
	const uint32_t id;
	AudioDecoder* const decoder;
	snd_pcm_t *handle;
	snd_async_handler_t *async_handler;
	snd_pcm_uframes_t frames_played;
	snd_pcm_uframes_t period_size;

	AlsaAudioStream(uint32_t _id, AudioDecoder *dec): id(_id), decoder(dec),
		handle(NULL), async_handler(NULL), frames_played(0) { }

	uint32_t getPlayedTime();
	void fill();
	void start();
	void stop();

	~AlsaAudioStream();
};

#endif
