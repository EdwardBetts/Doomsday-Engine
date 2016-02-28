/** @file game_init.cpp  Routines for initializing a game.
 *
 * @authors Copyright (c) 2003-2016 Jaakko Keränen <jaakko.keranen@iki.fi>
 * @authors Copyright (c) 2005-2015 Daniel Swanson <danij@dengine.net>
 *
 * @par License
 * GPL: http://www.gnu.org/licenses/gpl.html
 *
 * <small>This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. This program is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details. You should have received a copy of the GNU
 * General Public License along with this program; if not, see:
 * http://www.gnu.org/licenses</small>
 */

#include "doomsday/doomsdayapp.h"
#include "doomsday/games.h"
#include "doomsday/busymode.h"
#include "doomsday/session.h"
#include "doomsday/console/var.h"
#include "doomsday/filesys/fs_main.h"
#include "doomsday/filesys/virtualmappings.h"
#include "doomsday/filesys/wad.h"
#include "doomsday/resource/bundles.h"
#include "doomsday/resource/manifest.h"
#include "doomsday/world/entitydef.h"

#include <de/NativeFile>
#include <de/findfile.h>
#include <de/memory.h>

using namespace de;

static void updateProgress(int progress)
{
    DENG2_FOR_EACH_OBSERVER(Games::ProgressAudience, i,
                            DoomsdayApp::games().audienceForProgress())
    {
        i->gameWorkerProgress(progress);
    }
}

int beginGameChangeBusyWorker(void *context)
{
    DoomsdayApp::GameChangeParameters &parms = *(DoomsdayApp::GameChangeParameters *) context;

    P_InitMapEntityDefs();
    if(parms.initiatedBusyMode)
    {
        updateProgress(200);
    }
    return 0;
}

static File1 *tryLoadFile(de::Uri const &search, size_t baseOffset = 0)
{
    auto &fs1 = App_FileSystem();
    try
    {
        FileHandle &hndl = fs1.openFile(search.path(), "rb", baseOffset, false /* no duplicates */);

        de::Uri foundFileUri = hndl.file().composeUri();
        LOG_VERBOSE("Loading \"%s\"...") << NativePath(foundFileUri.asText()).pretty().toUtf8().constData();

        fs1.index(hndl.file());

        return &hndl.file();
    }
    catch(FS1::NotFoundError const&)
    {
        if(fs1.accessFile(search))
        {
            // Must already be loaded.
            LOG_RES_XVERBOSE("\"%s\" already loaded") << NativePath(search.asText()).pretty();
        }
    }
    return nullptr;
}

File1 *de::File1::tryLoad(Uri const &search, size_t baseOffset)
{
    return tryLoadFile(search, baseOffset);
}

static void loadResource(ResourceManifest &manifest)
{
    DENG2_ASSERT(manifest.resourceClass() == RC_PACKAGE);

    de::Uri path(manifest.resolvedPath(false/*do not locate resource*/), RC_NULL);
    if(path.isEmpty()) return;

    if(File1 *file = tryLoadFile(path))
    {
        // Mark this as an original game resource.
        file->setCustom(false);

        // Print the 'CRC' number of IWADs, so they can be identified.
        if(Wad *wad = file->maybeAs<Wad>())
        {
            LOG_RES_MSG("IWAD identification: %08x") << wad->calculateCRC();
        }
    }
}

static void parseStartupFilePathsAndAddFiles(char const *pathString)
{
    static char const *ATWSEPS = ",; \t";

    if(!pathString || !pathString[0]) return;

    size_t len = strlen(pathString);
    char *buffer = (char *) M_Malloc(len + 1);

    strcpy(buffer, pathString);
    char *token = strtok(buffer, ATWSEPS);
    while(token)
    {
        tryLoadFile(de::Uri(token, RC_NULL));
        token = strtok(nullptr, ATWSEPS);
    }
    M_Free(buffer);
}

static dint addListFiles(QStringList const &list, FileType const &ftype)
{
    dint numAdded = 0;
    foreach(QString const &path, list)
    {
        if(&ftype != &DD_GuessFileTypeFromFileName(path))
        {
            continue;
        }

        if(tryLoadFile(de::Uri(path, RC_NULL)))
        {
            numAdded += 1;
        }
    }
    return numAdded;
}

int loadGameStartupResourcesBusyWorker(void *context)
{
    DoomsdayApp::GameChangeParameters &parms = *(DoomsdayApp::GameChangeParameters *) context;

    // Reset file Ids so previously seen files can be processed again.
    App_FileSystem().resetFileIds();
    FS_InitVirtualPathMappings();
    App_FileSystem().resetAllSchemes();

    if(parms.initiatedBusyMode)
    {
        updateProgress(50);
    }

    if(App_GameLoaded())
    {
        // Create default Auto mappings in the runtime directory.

        // Data class resources.
        App_FileSystem().addPathMapping("auto/", de::Uri("$(App.DataPath)/$(GamePlugin.Name)/auto/", RC_NULL).resolved());

        // Definition class resources.
        App_FileSystem().addPathMapping("auto/", de::Uri("$(App.DefsPath)/$(GamePlugin.Name)/auto/", RC_NULL).resolved());
    }

    // Load data files.
    for(DataBundle const *bundle : DoomsdayApp::bundles().loaded())
    {
        LOG_RES_NOTE("Loading %s from %s") << bundle->description()
                                           << bundle->sourceFile().description();

        if(NativeFile const *nativeFile = bundle->sourceFile().maybeAs<NativeFile>())
        {
            if(File1 *file = File1::tryLoad(de::Uri::fromNativePath(nativeFile->nativePath())))
            {
                file->setCustom(false);
                LOG_RES_VERBOSE("%s: ok") << nativeFile->nativePath();
            }
            else
            {
                LOG_RES_WARNING("%s: could not load file") << nativeFile->nativePath();
            }
        }
    }

    /**
     * Open all the files, load headers, count lumps, etc, etc...
     * @note  Duplicate processing of the same file is automatically guarded
     *        against by the virtual file system layer.
     */
    GameManifests const &gameManifests = DoomsdayApp::game().manifests();
    int const numPackages = gameManifests.count(RC_PACKAGE);
    if(numPackages)
    {
        LOG_RES_MSG("Loading game resources...");

        int packageIdx = 0;
        for(GameManifests::const_iterator i = gameManifests.find(RC_PACKAGE);
            i != gameManifests.end() && i.key() == RC_PACKAGE; ++i, ++packageIdx)
        {
            loadResource(**i);

            // Update our progress.
            if(parms.initiatedBusyMode)
            {
                updateProgress((packageIdx + 1) * (200 - 50) / numPackages - 1);
            }
        }
    }

    if(parms.initiatedBusyMode)
    {
        updateProgress(200);
    }

    return 0;
}

/**
 * Find all game data file paths in the auto directory with the extensions
 * wad, lmp, pk3, zip and deh.
 *
 * @param found  List of paths to be populated.
 *
 * @return  Number of paths added to @a found.
 */
static dint findAllGameDataPaths(FS1::PathList &found)
{
    static String const extensions[] = {
        "wad", "lmp", "pk3", "zip", "deh"
#ifdef UNIX
        "WAD", "LMP", "PK3", "ZIP", "DEH" // upper case alternatives
#endif
    };
    dint const numFoundSoFar = found.count();
    for(String const &ext : extensions)
    {
        DENG2_ASSERT(!ext.isEmpty());
        String const searchPath = de::Uri(Path("$(App.DataPath)/$(GamePlugin.Name)/auto/*." + ext)).resolved();
        App_FileSystem().findAllPaths(searchPath, 0, found);
    }
    return found.count() - numFoundSoFar;
}

/**
 * Find and try to load all game data file paths in auto directory.
 *
 * @return Number of new files that were loaded.
 */
static dint loadFilesFromDataGameAuto()
{
    FS1::PathList found;
    findAllGameDataPaths(found);

    dint numLoaded = 0;
    DENG2_FOR_EACH_CONST(FS1::PathList, i, found)
    {
        // Ignore directories.
        if(i->attrib & A_SUBDIR) continue;

        if(tryLoadFile(de::Uri(i->path, RC_NULL)))
        {
            numLoaded += 1;
        }
    }
    return numLoaded;
}

/**
 * Looks for new files to autoload from the auto-load data directory.
 */
static void autoLoadFiles()
{
    /**
     * Keep loading files if any are found because virtual files may now
     * exist in the auto-load directory.
     */
    dint numNewFiles;
    while((numNewFiles = loadFilesFromDataGameAuto()) > 0)
    {
        LOG_RES_VERBOSE("Autoload round completed with %i new files") << numNewFiles;
    }
}

int loadAddonResourcesBusyWorker(void *context)
{
    DoomsdayApp::GameChangeParameters &parms = *(DoomsdayApp::GameChangeParameters *) context;

    // User-selected files specified in the game itself.
    for(String const &path : DoomsdayApp::game().userFiles())
    {
        NativePath const nativePath(path);
        Session::profile().resourceFiles << nativePath.withSeparators('/');
    }

    char const *startupFiles = CVar_String(Con_FindVariable("file-startup"));

    /**
     * Add additional game-startup files.
     * @note These must take precedence over Auto but not game-resource files.
     */
    if(startupFiles && startupFiles[0])
    {
        parseStartupFilePathsAndAddFiles(startupFiles);
    }

    if(parms.initiatedBusyMode)
    {
        updateProgress(50);
    }

    if(App_GameLoaded())
    {
        /**
         * Phase 3: Add real files from the Auto directory.
         */
        Session::Profile &prof = Session::profile();

        FS1::PathList found;
        findAllGameDataPaths(found);
        DENG2_FOR_EACH_CONST(FS1::PathList, i, found)
        {
            // Ignore directories.
            if(i->attrib & A_SUBDIR) continue;

            /// @todo Is expansion of symbolics still necessary here?
            prof.resourceFiles << NativePath(i->path).expand().withSeparators('/');
        }

        if(!prof.resourceFiles.isEmpty())
        {
            // First ZIPs then WADs (they may contain WAD files).
            addListFiles(prof.resourceFiles, DD_FileTypeByName("FT_ZIP"));
            addListFiles(prof.resourceFiles, DD_FileTypeByName("FT_WAD"));
        }

        // Final autoload round.
        autoLoadFiles();
    }

    if(parms.initiatedBusyMode)
    {
        updateProgress(180);
    }

    FS_InitPathLumpMappings();

    // Re-initialize the resource locator as there are now new resources to be found
    // on existing search paths (probably that is).
    App_FileSystem().resetAllSchemes();

    if(parms.initiatedBusyMode)
    {
        updateProgress(200);
    }

    return 0;
}