/** @file dam_main.cpp Doomsday Archived Map (DAM), map management. 
 *
 * @authors Copyright &copy; 2007-2013 Daniel Swanson <danij@dengine.net>
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
 * General Public License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA</small>
 */

#include <cmath>

#include "de_base.h"
#include "de_console.h"
#include "de_dam.h"
#include "de_defs.h"
#include "de_edit.h"
#include "de_filesys.h"
#include "de_network.h"
#include "de_render.h"

#include "map/gamemap.h"

#include "map/dam_main.h"

using namespace de;

/*
// Should we be caching successfully loaded maps?
byte mapCache = true;

static char const *mapCacheDir = "mapcache/";
*/

static archivedmap_t **archivedMaps;
static uint numArchivedMaps;

static void freeArchivedMap(archivedmap_t &dam);

static void clearArchivedMaps()
{
    if(archivedMaps)
    {
        for(uint i = 0; i < numArchivedMaps; ++i)
        {
            freeArchivedMap(*archivedMaps[i]);
        }
        Z_Free(archivedMaps);
        archivedMaps = 0;
    }
    numArchivedMaps = 0;
}

void DAM_Register()
{
    //C_VAR_BYTE("map-cache", &mapCache, 0, 0, 1);
}

void DAM_Init()
{
    // Allow re-init.
    clearArchivedMaps();
}

void DAM_Shutdown()
{
    clearArchivedMaps();
}

/**
 * Allocate memory for a new archived map record.
 */
static archivedmap_t *allocArchivedMap()
{
    return (archivedmap_t *) Z_Calloc(sizeof(archivedmap_t), PU_APPSTATIC, 0);
}

/// Free all memory acquired for an archived map record.
static void freeArchivedMap(archivedmap_t &dam)
{
    Uri_Delete(dam.uri);
    //Str_Free(&dam.cachedMapPath);
    Z_Free(&dam);
}

/// Create a new archived map record.
static archivedmap_t *createArchivedMap(de::Uri const &uri/*, ddstring_t const *cachedMapPath*/)
{
    archivedmap_t *dam = allocArchivedMap();

    dam->uri = reinterpret_cast<uri_s *>(new de::Uri(uri));
    /*dam->lastLoadAttemptFailed = false;
    Str_Init(&dam->cachedMapPath);
    Str_Set(&dam->cachedMapPath, Str_Text(cachedMapPath));

    if(DAM_MapIsValid(Str_Text(&dam->cachedMapPath),
                      F_LumpNumForName(uri.path().toString().toLatin1().constData())))
    {
        dam->cachedMapFound = true;
    }*/

    LOG_DEBUG("Added record for map '%s'.") << uri;

    return dam;
}

/**
 * Search the list of maps for one matching the specified identifier.
 *
 * @param uri  Identifier of the map to be searched for.
 * @return  The found map else @c NULL.
 */
static archivedmap_t *findArchivedMap(de::Uri const &uri)
{
    if(numArchivedMaps)
    {
        for(archivedmap_t **p = archivedMaps; *p; p++)
        {
            archivedmap_t *dam = *p;
            if(uri == reinterpret_cast<de::Uri &>(*dam->uri))
            {
                return dam;
            }
        }
    }
    return 0;
}

/**
 * Add an archived map to the list of maps.
 */
static void addArchivedMap(archivedmap_t *dam)
{
    archivedMaps = (archivedmap_t **) Z_Realloc(archivedMaps, sizeof(archivedmap_t *) * (++numArchivedMaps + 1), PU_APPSTATIC);
    archivedMaps[numArchivedMaps - 1] = dam;
    archivedMaps[numArchivedMaps] = 0; // Terminate.
}

#if 0

/// Calculate the identity key for maps loaded from this path.
static ushort calculateIdentifierForMapPath(char const *path)
{
    DENG_ASSERT(path && path[0]);

    ushort identifier = 0;
    for(uint i = 0; path[i]; ++i)
    {
        identifier ^= path[i] << ((i * 3) % 11);
    }
    return identifier;
}

AutoStr *DAM_ComposeCacheDir(char const *sourcePath)
{
    if(!sourcePath || !sourcePath[0]) return 0;

    DENG_ASSERT(App_GameLoaded());

    ddstring_t const *gameIdentityKey = App_CurrentGame().identityKey();
    ushort mapPathIdentifier   = calculateIdentifierForMapPath(sourcePath);

    AutoStr *mapFileName = AutoStr_NewStd();
    F_FileName(mapFileName, sourcePath);

    // Compose the final path.
    AutoStr *path = AutoStr_NewStd();
    Str_Appendf(path, "%s%s/%s-%04X/", mapCacheDir, Str_Text(gameIdentityKey),
                                       Str_Text(mapFileName), mapPathIdentifier);
    F_ExpandBasePath(path, path);

    return path;
}

static bool loadMap(GameMap **map, archivedmap_t *dam)
{
    *map = new GameMap;
    return DAM_MapRead(*map, Str_Text(&dam->cachedMapPath));
}
#endif

static bool convertMap(GameMap **map, archivedmap_t *dam)
{
    bool converted = false;

    VERBOSE(
        AutoStr *path = Uri_ToString(dam->uri);
        Con_Message("convertMap: Attempting conversion of '%s'.", Str_Text(path));
        );

    // Any converters available?
    if(Plug_CheckForHook(HOOK_MAP_CONVERT))
    {
        // Ask each converter in turn whether the map format is recognised
        // and if so, to interpret/transfer it to us via the map edit interface.
        if(DD_CallHooks(HOOK_MAP_CONVERT, 0, (void*) dam->uri))
        {
            // Transfer went OK.
            // Were we able to produce a valid map?
            converted = MPE_GetLastBuiltMapResult();
            if(converted)
            {
                *map = MPE_GetLastBuiltMap();
            }
        }
    }

    if(!converted || verbose >= 2)
        Con_Message("convertMap: %s.", (converted? "Successful" : "Failed"));

    return converted;
}

static String DAM_ComposeUniqueId(de::File1 &markerLump)
{
    return String("%1|%2|%3|%4")
              .arg(markerLump.name().fileNameWithoutExtension())
              .arg(markerLump.container().name().fileNameWithoutExtension())
              .arg(markerLump.container().hasCustom()? "pwad" : "iwad")
              .arg(Str_Text(App_CurrentGame().identityKey()))
              .toLower();
}

/**
 * Attempt to load the map associated with the specified identifier.
 */
boolean DAM_AttemptMapLoad(uri_s const *_uri)
{
    DENG_ASSERT(_uri);
    de::Uri const &uri = reinterpret_cast<de::Uri const &>(*_uri);

    bool loadedOK = false;

    LOG_VERBOSE("Attempting to load map '%s'...") << uri;

    archivedmap_t *dam = findArchivedMap(uri);
    if(!dam)
    {
        // We've not yet attempted to load this map.
        lumpnum_t markerLumpNum = F_LumpNumForName(uri.path().toString().toLatin1().constData());
        if(0 > markerLumpNum) return false;

/*
        // Compose the cache directory path and ensure it exists.
        AutoStr *cachedMapDir = DAM_ComposeCacheDir(Str_Text(F_ComposeLumpFilePath(markerLumpNum)));
        F_MakePath(Str_Text(cachedMapDir));

        // Compose the full path to the cached map data file.
        AutoStr *cachedMapPath = AutoStr_NewStd();
        F_FileName(cachedMapPath, Str_Text(F_LumpName(markerLumpNum)));
        Str_Append(cachedMapPath, ".dcm");
        Str_Prepend(cachedMapPath, Str_Text(cachedMapDir));
*/

        // Create an archived map record for this.
        dam = createArchivedMap(uri/*, cachedMapPath*/);
        addArchivedMap(dam);
    }

    // Load it in.
    //if(!dam->lastLoadAttemptFailed)
    {
        GameMap *map = 0;

        Z_FreeTags(PU_MAP, PU_PURGELEVEL - 1);

        /*if(mapCache && dam->cachedMapFound)
        {
            // Attempt to load the cached map data.
            if(loadMap(&map, dam))
                loadedOK = true;
        }
        else*/
        {
            // Try a JIT conversion with the help of a plugin.
            if(convertMap(&map, dam))
                loadedOK = true;
        }

        if(loadedOK)
        {
            // Do any initialization/error checking work we need to do.
            // Must be called before we go any further.
            P_InitUnusedMobjList();

            // Must be called before any mobjs are spawned.
            map->initNodePiles();

#ifdef __CLIENT__
            // Prepare the client-side data.
            if(isClient)
            {
                map->initClMobjs();
            }
#endif

            Rend_DecorInit();

            vec2d_t min, max;
            map->bounds(min, max);

            // Init blockmap for searching BSP leafs.
            map->initBspLeafBlockmap(min, max);
            foreach(BspLeaf *bspLeaf, map->bspLeafs())
            {
                map->linkBspLeaf(*bspLeaf);
            }

            map->_uri = *reinterpret_cast<de::Uri *>(dam->uri);

            // Generate the unique map id.
            lumpnum_t markerLumpNum = App_FileSystem().lumpNumForName(Str_Text(Uri_Path(dam->uri)));
            de::File1 &markerLump   = App_FileSystem().nameIndex().lump(markerLumpNum);

            String uniqueId     = DAM_ComposeUniqueId(markerLump);
            QByteArray uniqueIdUtf8 = uniqueId.toUtf8();
            qstrncpy(map->_oldUniqueId, uniqueIdUtf8.constData(), sizeof(map->_oldUniqueId));

            // See what mapinfo says about this map.
            ded_mapinfo_t *mapInfo = Def_GetMapInfo(dam->uri);
            if(!mapInfo)
            {
                de::Uri mapUri("*", RC_NULL);
                mapInfo = Def_GetMapInfo(reinterpret_cast<uri_s *>(&mapUri));
            }

#ifdef __CLIENT__
            ded_sky_t *skyDef = 0;
            if(mapInfo)
            {
                skyDef = Def_GetSky(mapInfo->skyID);
                if(!skyDef)
                    skyDef = &mapInfo->sky;
            }
            Sky_Configure(skyDef);
#endif

            // Setup accordingly.
            if(mapInfo)
            {
                map->_globalGravity = mapInfo->gravity;
                map->_ambientLightLevel = mapInfo->ambient * 255;
            }
            else
            {
                // No map info found, so set some basic stuff.
                map->_globalGravity = 1.0f;
                map->_ambientLightLevel = 0;
            }

            map->_effectiveGravity = map->_globalGravity;

            /// @todo Should be done in P_LoadMap() (note: R_InitMap)
            theMap = map;

#ifdef __CLIENT__
            Rend_RadioInitForMap();
#endif

            map->initSkyFix();
        }
    }

/*  if(!loadedOK)
        dam->lastLoadAttemptFailed = true;
*/

    return loadedOK;
}
