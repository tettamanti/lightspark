/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009,2010  Alessandro Pignotti (a.pignotti@sssup.it)
    Copyright (C) 2010 Alexandre Demers (papouta@hotmail.com)

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

#ifndef AUDIOPLUGIN_H
#define AUDIOPLUGIN_H

#include "../../../compat.h"
#include "../../decoder.h"
#include "../IPlugin.h"
#include <iostream>

using std::string;
using std::list;

class AudioStream; //Forward declaration, it will be implemented per plugin

/**********************
Abstract class for audio plugin implementation
***********************/
class AudioPlugin : public IPlugin
{
protected:
	string playbackDeviceName;
	string captureDeviceName;
	list<AudioStream *> streams;
	typedef list<AudioStream *>::iterator stream_iterator;
	bool stopped;
	
	AudioPlugin(string plugin_name, string backend_name, bool init_stopped = false):
			IPlugin(AUDIO, plugin_name, backend_name), stopped(init_stopped) {}

public:
	enum DEVICE_TYPES { PLAYBACK, CAPTURE };

	virtual bool isStopped() const {
		return stopped;
	}
	virtual bool isTimingAvailable() const = 0;

	virtual AudioStream *createStream(lightspark::AudioDecoder *decoder) = 0;
	virtual void freeStream(AudioStream *stream) = 0;
	virtual uint32_t getPlayedTime(AudioStream *stream) = 0;

	virtual void stop() = 0;

	virtual ~AudioPlugin() { };
};

#endif
