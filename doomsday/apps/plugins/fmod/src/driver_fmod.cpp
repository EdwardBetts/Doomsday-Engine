/** @file driver_fmod.cpp  FMOD Ex audio plugin.
 * @ingroup dsfmod
 *
 * @authors Copyright © 2011-2013 Jaakko Keränen <jaakko.keranen@iki.fi>
 * @authors Copyright © 2015 Daniel Swanson <danij@dengine.net>
  *
 * @par License
 * GPL: http://www.gnu.org/licenses/gpl.html (with exception granted to allow
 * linking against FMOD Ex)
 *
 * <small>This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. This program is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details. You should have received a copy of the GNU
 * General Public License along with this program; if not, see:
 * http://www.gnu.org/licenses
 *
 * <b>Special Exception to GPLv2:</b>
 * Linking the Doomsday Audio Plugin for FMOD Ex (audio_fmod) statically or
 * dynamically with other modules is making a combined work based on
 * audio_fmod. Thus, the terms and conditions of the GNU General Public License
 * cover the whole combination. In addition, <i>as a special exception</i>, the
 * copyright holders of audio_fmod give you permission to combine audio_fmod
 * with free software programs or libraries that are released under the GNU
 * LGPL and with code included in the standard release of "FMOD Ex Programmer's
 * API" under the "FMOD Ex Programmer's API" license (or modified versions of
 * such code, with unchanged license). You may copy and distribute such a
 * system following the terms of the GNU GPL for audio_fmod and the licenses of
 * the other code concerned, provided that you include the source code of that
 * other code when and as the GNU GPL requires distribution of source code.
 * (Note that people who make modified versions of audio_fmod are not obligated
 * to grant this special exception for their modified versions; it is their
 * choice whether to do so. The GNU General Public License gives permission to
 * release a modified version without this exception; this exception also makes
 * it possible to release a modified version which carries forward this
 * exception.) </small>
 */

#include "driver_fmod.h"

#include "api_audiod.h"
#include "api_audiod_sfx.h"
#include "doomsday.h"
#include <de/c_wrapper.h>

#include <cstdio>
#include <fmod.h>
#include <fmod_errors.h>
#include <fmod.hpp>

FMOD::System *fmodSystem;

static FMOD_RESULT result;

/**
 * Initialize the FMOD Ex sound driver.
 */
int DS_Init()
{
    // Already been here?
    if(::fmodSystem) return true;

    // Create the FMOD audio system.
    if((::result = FMOD::System_Create(&::fmodSystem)) != FMOD_OK)
    {
        LOGDEV_AUDIO_ERROR("FMOD::System_Create failed (%d) %s")
            << ::result << FMOD_ErrorString(::result);
        ::fmodSystem = nullptr;
        return false;
    }

#ifdef WIN32
    // Figure out the system's configured speaker mode.
    FMOD_SPEAKERMODE speakerMode;
    if((::result = ::fmodSystem->getDriverCaps(0, 0, 0, &speakerMode)) == FMOD_OK)
    {
        ::fmodSystem->setSpeakerMode(speakerMode);
    }
#endif

    // Manual overrides.
    if(CommandLine_Exists("-speaker51"))
    {
        ::fmodSystem->setSpeakerMode(FMOD_SPEAKERMODE_5POINT1);
    }
    if(CommandLine_Exists("-speaker71"))
    {
        ::fmodSystem->setSpeakerMode(FMOD_SPEAKERMODE_7POINT1);
    }
    if(CommandLine_Exists("-speakerprologic"))
    {
        ::fmodSystem->setSpeakerMode(FMOD_SPEAKERMODE_SRS5_1_MATRIX);
    }

    // Initialize FMOD.
    if((::result = ::fmodSystem->init(50, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_HRTF_LOWPASS, 0)) != FMOD_OK)
    {
        LOGDEV_AUDIO_ERROR("FMOD init failed: (%d) %s")
            << ::result << FMOD_ErrorString(::result);

        ::fmodSystem->release();
        ::fmodSystem = nullptr;
        return false;
    }

    // Options.
    FMOD_ADVANCEDSETTINGS settings; zeroStruct(settings);
    settings.HRTFMaxAngle = 360;
    settings.HRTFMinAngle = 180;
    settings.HRTFFreq     = 11000;
    ::fmodSystem->setAdvancedSettings(&settings);

#ifdef DENG2_DEBUG
    int numPlugins = 0;
    ::fmodSystem->getNumPlugins(FMOD_PLUGINTYPE_CODEC, &numPlugins);
    DSFMOD_TRACE("Plugins loaded: " << numPlugins);
    for(int i = 0; i < numPlugins; ++i)
    {
        uint handle;
        ::fmodSystem->getPluginHandle(FMOD_PLUGINTYPE_CODEC, i, &handle);

        FMOD_PLUGINTYPE pType;
        char pName[100];
        uint pVer = 0;
        ::fmodSystem->getPluginInfo(handle, &pType, pName, sizeof(pName), &pVer);

        DSFMOD_TRACE("Plugin " << i << ", handle " << handle << ": type " << pType
                     << ", name:'" << pName << "', ver:" << pVer);
    }
#endif

    // Print the credit required by FMOD license.
    LOG_AUDIO_NOTE("FMOD Sound System (c) Firelight Technologies Pty, Ltd., 1994-2013");

    LOGDEV_AUDIO_VERBOSE("[FMOD] Initialized");
    return true;
}

/**
 * Shut everything down.
 */
void DS_Shutdown()
{
    DMFmod_Music_Shutdown();
    DMFmod_CDAudio_Shutdown();

    DSFMOD_TRACE("DS_Shutdown.");
    ::fmodSystem->release();
    ::fmodSystem = nullptr;
}

/**
 * The Event function is called to tell the driver about certain critical
 * events like the beginning and end of an update cycle.
 */
void DS_Event(int type)
{
    if(!::fmodSystem) return;

    if(type == SFXEV_END)
    {
        // End of frame, do an update.
        ::fmodSystem->update();
    }
}

int DS_Get(int prop, void *ptr)
{
    switch(prop)
    {
    case AUDIOP_IDENTIFIER: {
        auto *id = reinterpret_cast<AutoStr *>(ptr);
        DENG2_ASSERT(id);
        if(id) Str_Set(id, "fmod");
        return true; }

    case AUDIOP_NAME: {
        auto *name = reinterpret_cast<AutoStr *>(ptr);
        DENG2_ASSERT(name);
        if(name) Str_Set(name, "FMOD");
        return true; }

    default: DSFMOD_TRACE("DS_Get: Unknown property " << prop); break;
    }
    return false;
}

int DS_Set(int prop, void const *ptr)
{
    if(::fmodSystem)
    {
        switch(prop)
        {
        case AUDIOP_SOUNDFONT_FILENAME: {
            auto const *path = reinterpret_cast<char const *>(ptr);
            DSFMOD_TRACE("DS_Set: Soundfont = " << path);
            if(!path || !qstrlen(path))
            {
                // Use the default.
                path = nullptr;
            }
            DMFmod_Music_SetSoundFont(path);
            return true; }

        default: DSFMOD_TRACE("DS_Set: Unknown property " << prop); break;
        }
    }

    return false;
}

/**
 * Declares the type of the plugin so the engine knows how to treat it. Called
 * automatically when the plugin is loaded.
 */
DENG_EXTERN_C char const *deng_LibraryType()
{
    return "deng-plugin/audio";
}

DENG_DECLARE_API(Con);

DENG_API_EXCHANGE(
    DENG_GET_API(DE_API_CONSOLE, Con);
)
