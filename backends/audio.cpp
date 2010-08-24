/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009,2010  Alessandro Pignotti (a.pignotti@sssup.it)

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
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "audio.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include "../logger.h"

//Needed or not with compat.h and compat.cpp?
#if defined WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <sys/types.h>
#endif

#ifdef ENABLE_SOUND
using namespace lightspark;
using namespace std;
using namespace boost::filesystem;
using namespace boost;


/****************
AudioManager::AudioManager
*****************
It should search for a list of audio plugin lib files (liblightsparkAUDIOAPIplugin.so)
Then, it should read a config file containing the user's defined audio API choosen as audio backend
If no file or none selected
  default to none
Else
  Select and load the good audio plugin lib files
*****************/

AudioManager::AudioManager ( PluginManager *sharedPluginManager )
{
        pluginManager = sharedPluginManager;
        selectedAudioBackend = "";
        oAudioPlugin = NULL;
//	  string DesiredAudio = get_audioConfig(); //Looks for the audio selected in the user's config
        string DesiredAudio = "pulse";
        set_audiobackend ( DesiredAudio );
}

void AudioManager::fillPlugin ( uint32_t id )
{
#if 0
        if ( oAudioPlugin != NULL ) {
                oAudioPlugin->fill ( id );
        } else {
                LOG ( LOG_ERROR,_ ( "No audio plugin loaded" ) );
        }
#endif
}

void AudioManager::freeStreamPlugin ( AudioStream *stream )
{
        if ( oAudioPlugin != NULL ) {
                oAudioPlugin->freeStream ( stream );
        } else {
                LOG ( LOG_ERROR,_ ( "No audio plugin loaded" ) );
        }
}

AudioStream *AudioManager::createStreamPlugin ( AudioDecoder *decoder )
{
        if ( oAudioPlugin != NULL ) {
                return oAudioPlugin->createStream ( decoder );
        } else {
                LOG ( LOG_ERROR,_ ( "No audio plugin loaded" ) );
                return NULL;
        }
}

uint32_t AudioManager::getPlayedTimePlugin ( AudioStream *stream )
{
        if ( oAudioPlugin != NULL ) {
                return oAudioPlugin->getPlayedTime ( stream );
        } else {
                LOG ( LOG_ERROR,_ ( "getPlayedTimePlugin: No audio plugin loaded" ) );
                return 0;
        }
}

bool AudioManager::isTimingAvailablePlugin() const
{
        if ( oAudioPlugin != NULL ) {
                return oAudioPlugin->isTimingAvailable();
        } else {
                LOG ( LOG_ERROR,_ ( "isTimingAvailablePlugin: No audio plugin loaded" ) );
                return false;
        }
}

void AudioManager::set_audiobackend ( string desired_backend )
{
        if ( selectedAudioBackend != desired_backend ) {	//Load the desired backend only if it's not already loaded
                load_audioplugin ( desired_backend );
                selectedAudioBackend = desired_backend;
        }
}

void AudioManager::get_audioBackendsList()
{
        audioplugins_list = pluginManager->get_backendsList ( AUDIO );
}

void AudioManager::refresh_audioplugins_list()
{
        audioplugins_list.clear();
        get_audioBackendsList();
}

void AudioManager::release_audioplugin()
{
        if ( oAudioPlugin != NULL ) {
                oAudioPlugin->stop();
                pluginManager->release_plugin ( oAudioPlugin );
        }
}

void AudioManager::load_audioplugin ( string selected_backend )
{
        LOG ( LOG_NO_INFO,_ ( ( ( string ) ( "the selected backend is: " + selected_backend ) ).c_str() ) );
        release_audioplugin();
        oAudioPlugin = static_cast<AudioPlugin *> ( pluginManager->get_plugin ( selected_backend ) );

        if ( oAudioPlugin == NULL ) {
                LOG ( LOG_ERROR,_ ( "Could not load the audiobackend" ) );
        }
}

/**************************
stop AudioManager
***************************/
AudioManager::~AudioManager()
{
        release_audioplugin();
        pluginManager = NULL;	//The plugin manager is not deleted since it's been created outside of the audio manager
}

void AudioManager::stopPlugin()
{
        if ( oAudioPlugin != NULL ) {
                oAudioPlugin->stop();
        }
}

#endif
