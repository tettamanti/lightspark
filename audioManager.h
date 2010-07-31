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

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "compat.h"
#include "decoder.h"
#include <string.h>
#include <boost/filesystem.hpp>

#include "plugins/audio/audioPlugin.h"

using namespace std;

//convenience typedef for the pointers to the 2 functions we expect to find in the plugin libraries
typedef IPlugin *(*PLUGIN_FACTORY)();
typedef void (*PLUGIN_CLEANUP)(IPlugin *);

namespace lightspark
{

class AudioManager
{
  private:
    class PluginInfo
    {
//      private:
      public:
	char *plugin_name;
	char *audiobackend_name;
	char *plugin_path;
	bool enabled;
	HLIB hAudioPlugin;
	PluginInfo *PreviousPluginLib;
	PluginInfo *NextPluginLib;
//      public:
	PluginInfo();
	~PluginInfo();
    };
    class PluginList
    {
      public:
	PluginInfo *FirstAudioPlugin;
	PluginInfo *LastAudioPlugin;
    };
    HLIB hSelectedAudioPluginLib;
    PluginList *AudioPluginsList;
    PluginInfo *SelectedAudioPlugin;
    
    AudioPlugin *o_AudioPlugin;
    void AddAudioPluginToList(const HLIB h_pluginToAdd, const char *pathToPlugin);
    void FindAudioPlugins();
    void LoadAudioPlugin();

  public:
    AudioManager();
    uint32_t createStreamPlugin(AudioDecoder *decoder);
    void freeStreamPlugin(uint32_t id);
    void fillAndSyncPlugin(uint32_t id, uint32_t streamTime);
    void stopPlugin();
    void set_audiobackend();
    ~AudioManager();
};

};

#endif