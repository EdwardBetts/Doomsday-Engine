/**\file p_saveio.c
 *\section License
 * License: GPL
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright © 2003-2012 Jaakko Keränen <jaakko.keranen@iki.fi>
 *\author Copyright © 2005-2012 Daniel Swanson <danij@dengine.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "dmu_lib.h"
#include "p_mapsetup.h"
#include "p_saveg.h"
#include "p_saveio.h"
#include "p_savedef.h"
#include "materialarchive.h"

static boolean inited;
static LZFILE* savefile;
static ddstring_t savePath; // e.g., "savegame/"
#if !__JHEXEN__
static ddstring_t clientSavePath; // e.g., "savegame/client/"
#endif
static gamesaveinfo_t* gameSaveInfo;

#if __JHEXEN__
static saveptr_t saveptr;
static void CopyFile(const char* sourceName, const char* destName);
#endif

static void errorIfNotInited(const char* callerName)
{
    if(inited) return;
    Con_Error("%s: Savegame I/O is not presently initialized.", callerName);
    // Unreachable. Prevents static analysers from getting rather confused, poor things.
    exit(1);
}

void SV_InitIO(void)
{
    Str_Init(&savePath);
#if !__JHEXEN__
    Str_Init(&clientSavePath);
#endif
    gameSaveInfo = NULL;
    inited = true;
    savefile = 0;
}

void SV_ShutdownIO(void)
{
    inited = false;

    SV_CloseFile();

    if(gameSaveInfo)
    {
        int i;
        for(i = 0; i < NUMSAVESLOTS; ++i)
        {
            gamesaveinfo_t* info = &gameSaveInfo[i];
            Str_Free(&info->filePath);
            Str_Free(&info->name);
        }
        free(gameSaveInfo); gameSaveInfo = NULL;
    }

    Str_Free(&savePath);
#if !__JHEXEN__
    Str_Free(&clientSavePath);
#endif
}

const char* SV_SavePath(void)
{
    return Str_Text(&savePath);
}

#ifdef __JHEXEN__
saveptr_t* SV_HxSavePtr(void)
{
    return &saveptr;
}
#endif

#if !__JHEXEN__
const char* SV_ClientSavePath(void)
{
    return Str_Text(&clientSavePath);
}
#endif

/// @return  Possibly relative saved game directory. Must be released with Str_Delete()
static ddstring_t* composeSaveDir(void)
{
    ddstring_t* dir = Str_New();
    if(ArgCheckWith("-savedir", 1))
    {
        Str_Set(dir, ArgNext());
        // Add a trailing backslash is necessary.
        if(Str_RAt(dir, 0) != '/')
            Str_AppendChar(dir, '/');
        return dir;
    }

    // Use the default path.
    { GameInfo gameInfo;
    if(DD_GameInfo(&gameInfo))
    {
#if __JHEXEN__
        Str_Appendf(dir, "hexndata/%s/", gameInfo.identityKey);
#else
        Str_Appendf(dir, "savegame/%s/", gameInfo.identityKey);
#endif
        return dir;
    }}

    Str_Delete(dir);
    Con_Error("composeSaveDir: Error, failed retrieving Game.");
    return NULL; // Unreachable.
}

// Compose and create the saved game directories.
void SV_ConfigureSavePaths(void)
{
    assert(inited);
    {
    ddstring_t* saveDir = composeSaveDir();
    boolean savePathExists;

    Str_Set(&savePath, Str_Text(saveDir));
#if !__JHEXEN__
    Str_Clear(&clientSavePath); Str_Appendf(&clientSavePath, "%sclient/", Str_Text(saveDir));
#endif
    Str_Delete(saveDir);

    // Ensure that these paths exist.
    savePathExists = F_MakePath(Str_Text(&savePath));
#if !__JHEXEN__
    if(!F_MakePath(Str_Text(&clientSavePath)))
        savePathExists = false;
#endif
    if(!savePathExists)
        Con_Message("Warning:configureSavePaths: Failed to locate \"%s\"\nPerhaps it could "
            "not be created (insufficent permissions?). Saving will not be possible.\n", Str_Text(&savePath));
    }
}

LZFILE* SV_File(void)
{
    return savefile;
}

LZFILE* SV_OpenFile(const char *fileName, const char* mode)
{
    assert(savefile == 0);
    savefile = lzOpen((char*)fileName, (char*)mode);
    return savefile;
}

void SV_CloseFile(void)
{
    if(savefile)
    {
        lzClose(savefile);
        savefile = 0;
    }
}


#if __JHEXEN__
boolean SV_ExistingFile(char *name)
{
    FILE *fp;

    if((fp = fopen(name, "rb")) != NULL)
    {
        fclose(fp);
        return true;
    }
    else
    {
        return false;
    }
}

void SV_ClearSaveSlot(int slot)
{
    ddstring_t fileName;
    Str_Init(&fileName);
    { int i;
    for(i = 0; i < MAX_MAPS; ++i)
    {
        Str_Clear(&fileName);
        Str_Appendf(&fileName, "%shex%d%02d.hxs", Str_Text(&savePath), slot, i);
        F_TranslatePath(&fileName, &fileName);
        remove(Str_Text(&fileName));
    }}
    Str_Clear(&fileName);
    Str_Appendf(&fileName, "%shex%d.hxs", Str_Text(&savePath), slot);
    F_TranslatePath(&fileName, &fileName);
    remove(Str_Text(&fileName));
    Str_Free(&fileName);
}
#endif

static void CopyFile(const char* sourceName, const char* destName)
{
    size_t length;
    char* buffer;
    LZFILE* outf;

    length = M_ReadFile(sourceName, &buffer);
    if(0 == length)
    {
        Con_Message("Warning:CopyFile: Failed opening \"%s\" for reading.\n", sourceName);
        return;
    }

    outf = lzOpen((char*)destName, "wp");
    if(NULL != outf)
    {
        lzWrite(buffer, length, outf);
        lzClose(outf);
    }
    Z_Free(buffer);
}

boolean SV_IsValidSlot(int slot)
{
#if __JHEXEN__
    if(slot == REBORN_SLOT) return true;
#endif
    return (slot >= 0  && slot < NUMSAVESLOTS);
}

static boolean readGameSaveInfoFromFile(const ddstring_t* savePath, ddstring_t* name)
{
    boolean found = false;
#if __JHEXEN__
    LZFILE* fp;
#endif
    assert(inited && savePath && name);

    if(Str_IsEmpty(savePath)) return false;

#if __JHEXEN__
    fp = lzOpen(Str_Text(savePath), "rp");
    if(fp)
    {
        // Read the header.
        char versionText[HXS_VERSION_TEXT_LENGTH];
        char nameBuffer[SAVESTRINGSIZE];
        lzRead(nameBuffer, SAVESTRINGSIZE, fp);
        lzRead(versionText, HXS_VERSION_TEXT_LENGTH, fp);
        lzClose(fp); fp = NULL;
        if(!strncmp(versionText, HXS_VERSION_TEXT, 8))
        {
            SV_SetSaveVersion(atoi(&versionText[8]));
            if(SV_SaveVersion() <= MY_SAVE_VERSION)
            {
                Str_Set(name, nameBuffer);
                found = true;
            }
        }
    }
#else
    if(SV_OpenFile(Str_Text(savePath), "rp"))
    {
        saveheader_t* hdr = SV_SaveHeader();
        // Read the header.
        lzRead(hdr, sizeof(*hdr), SV_File());
        SV_CloseFile();
        if(MY_SAVE_MAGIC == hdr->magic)
        {
            Str_Set(name, hdr->name);
            found = true;
        }
    }

    // If not found or not recognized try other supported formats.
#if !__JDOOM64__
    if(!found)
    {
        // Perhaps a DOOM(2).EXE v19 saved game?
        if(SV_OpenFile(Str_Text(savePath), "r"))
        {
            char nameBuffer[SAVESTRINGSIZE];
            lzRead(nameBuffer, SAVESTRINGSIZE, SV_File());
            nameBuffer[SAVESTRINGSIZE - 1] = 0;
            Str_Set(name, nameBuffer);
            SV_CloseFile();
            found = true;
        }
    }
# endif
#endif

    // Ensure we have a non-empty name.
    if(found && Str_IsEmpty(name))
    {
        Str_Set(name, "UNNAMED");
    }

    return found;
}

/**
 * Compose the (possibly relative) path to the game-save associated
 * with the logical save @a slot. If the game-save path is unreachable
 * then @a path will be made empty.
 *
 * @param slot  Logical save slot identifier.
 * @param path  String buffer to populate with the game save path.
 * @return  @c true if @a path was set.
 */
static boolean composeGameSavePathForSlot(int slot, ddstring_t* path)
{
    assert(inited && SV_IsValidSlot(slot) && path);
    // Do we have a valid path?
    if(!F_MakePath(SV_SavePath())) return false;
    // Compose the full game-save path and filename.
    Str_Clear(path);
    Str_Appendf(path, "%s" SAVEGAMENAME "%i." SAVEGAMEEXTENSION, SV_SavePath(), slot);
    F_TranslatePath(path, path);
    return true;
}

/// Re-build game-save info by re-scanning the save paths and populating the list.
static void buildGameSaveInfo(void)
{
    int i;
    assert(inited);

    if(!gameSaveInfo)
    {
        // Not yet been here. We need to allocate and initialize the game-save info list.
        gameSaveInfo = (gamesaveinfo_t*) malloc(NUMSAVESLOTS * sizeof(*gameSaveInfo));
        if(!gameSaveInfo)
            Con_Error("buildGameSaveInfo: Failed on allocation of %lu bytes for "
                "game-save info list.", (unsigned long) (NUMSAVESLOTS * sizeof(*gameSaveInfo)));

        // Initialize.
        for(i = 0; i < NUMSAVESLOTS; ++i)
        {
            gamesaveinfo_t* info = &gameSaveInfo[i];
            Str_Init(&info->filePath);
            Str_Init(&info->name);
            info->slot = i;
        }
    }

    /// Scan the save paths and populate the list.
    /// \todo We should look at all files on the save path and not just those
    /// which match the default game-save file naming convention.
    for(i = 0; i < NUMSAVESLOTS; ++i)
    {
        gamesaveinfo_t* info = &gameSaveInfo[i];

        composeGameSavePathForSlot(i, &info->filePath);
        if(Str_IsEmpty(&info->filePath))
        {
            // The save path cannot be accessed for some reason. Perhaps its a
            // network path? Clear the info for this slot.
            Str_Clear(&info->name);
            continue;
        }

        if(!readGameSaveInfoFromFile(&info->filePath, &info->name))
        {
            // Not a valid save file.
            Str_Clear(&info->filePath);
        }
    }
}

/// Given a logical save slot identifier retrieve the assciated game-save info.
static gamesaveinfo_t* findGameSaveInfoForSlot(int slot)
{
    static gamesaveinfo_t invalidInfo = { { "" }, { "" }, -1 };
    assert(inited);

    if(slot >= 0 && slot < NUMSAVESLOTS)
    {
        // On first call - automatically build and populate game-save info.
        if(!gameSaveInfo)
            buildGameSaveInfo();
        // Retrieve the info for this slot.
        return &gameSaveInfo[slot];
    }
    return &invalidInfo;
}

boolean SV_IsGameSaveSlotUsed(int slot)
{
    const gamesaveinfo_t* info;
    errorIfNotInited("SV_IsGameSaveSlotUsed");
    info = findGameSaveInfoForSlot(slot);
    return !Str_IsEmpty(&info->filePath);
}

const gamesaveinfo_t* SV_GetGameSaveInfoForSlot(int slot)
{
    errorIfNotInited("SV_GetGameSaveInfoForSlot");
    return findGameSaveInfoForSlot(slot);
}

void SV_UpdateGameSaveInfo(void)
{
    errorIfNotInited("SV_UpdateGameSaveInfo");
    buildGameSaveInfo();
}

int SV_ParseGameSaveSlot(const char* str)
{
    int slot;

    // Try game-save name match.
    slot = SV_FindGameSaveSlotForName(str);
    if(slot >= 0)
    {
        return slot;
    }

    // Try keyword identifiers.
    if(!stricmp(str, "<quick>"))
    {
        return Con_GetInteger("game-save-quick-slot");
    }

    // Try logical slot identifier.
    if(M_IsStringValidInt(str))
    {
        return atoi(str);
    }

    // Unknown/not found.
    return -1;
}

int SV_FindGameSaveSlotForName(const char* name)
{
    int saveSlot = -1;

    errorIfNotInited("SV_FindGameSaveSlotForName");

    if(name && name[0])
    {
        int i = 0;
        // On first call - automatically build and populate game-save info.
        if(!gameSaveInfo)
        {
            buildGameSaveInfo();
        }

        do
        {
            const gamesaveinfo_t* info = &gameSaveInfo[i];
            if(!Str_CompareIgnoreCase(&info->name, name))
            {
                // This is the one!
                saveSlot = i;
            }
        } while(-1 == saveSlot && ++i < NUMSAVESLOTS);
    }
    return saveSlot;
}

boolean SV_GetGameSavePathForSlot(int slot, ddstring_t* path)
{
    errorIfNotInited("SV_GetGameSavePathForSlot");
    if(!path) return false;

    Str_Clear(path);
    if(!SV_IsValidSlot(slot)) return false;
    return composeGameSavePathForSlot(slot, path);
}

#if __JHEXEN__
void SV_CopySaveSlot(int sourceSlot, int destSlot)
{
    ddstring_t src, dst;

    Str_Init(&src);
    Str_Init(&dst);

    { int i;
    for(i = 0; i < MAX_MAPS; ++i)
    {
        Str_Clear(&src);
        Str_Appendf(&src, "%shex%d%02d.hxs", Str_Text(&savePath), sourceSlot, i);
        F_TranslatePath(&src, &src);

        if(SV_ExistingFile(Str_Text(&src)))
        {
            Str_Clear(&dst);
            Str_Appendf(&dst, "%shex%d%02d.hxs", Str_Text(&savePath), destSlot, i);
            F_TranslatePath(&dst, &dst);
            CopyFile(Str_Text(&src), Str_Text(&dst));
        }
    }}

    Str_Clear(&src);
    Str_Appendf(&src, "%shex%d.hxs", Str_Text(&savePath), sourceSlot);
    F_TranslatePath(&src, &src);

    if(SV_ExistingFile(Str_Text(&src)))
    {
        Str_Clear(&dst);
        Str_Appendf(&dst, "%shex%d.hxs", Str_Text(&savePath), destSlot);
        F_TranslatePath(&dst, &dst);
        CopyFile(Str_Text(&src), Str_Text(&dst));
    }

    Str_Free(&dst);
    Str_Free(&src);
}

/**
 * Copies the base slot to the reborn slot.
 */
void SV_HxUpdateRebornSlot(void)
{
    errorIfNotInited("SV_HxUpdateRebornSlot");
    SV_ClearSaveSlot(REBORN_SLOT);
    SV_CopySaveSlot(BASE_SLOT, REBORN_SLOT);
}

void SV_HxClearRebornSlot(void)
{
    errorIfNotInited("SV_HxClearRebornSlot");
    SV_ClearSaveSlot(REBORN_SLOT);
}

int SV_HxGetRebornSlot(void)
{
    errorIfNotInited("SV_HxGetRebornSlot");
    return (REBORN_SLOT);
}

boolean SV_HxRebornSlotAvailable(void)
{
    boolean result;
    ddstring_t path;
    errorIfNotInited("SV_HxRebornSlotAvailable");
    Str_Init(&path);
    Str_Appendf(&path, "%shex%d.hxs", Str_Text(&savePath), REBORN_SLOT);
    F_TranslatePath(&path, &path);
    result = SV_ExistingFile(Str_Text(&path));
    Str_Free(&path);
    return result;
}

void SV_HxInitBaseSlot(void)
{
    errorIfNotInited("SV_HxInitBaseSlot");
    SV_ClearSaveSlot(BASE_SLOT);
}
#endif

void SV_Write(const void* data, int len)
{
    errorIfNotInited("SV_Write");
    lzWrite((void*)data, len, savefile);
}

void SV_WriteByte(byte val)
{
    errorIfNotInited("SV_WriteByte");
    lzPutC(val, savefile);
}

#if __JHEXEN__
void SV_WriteShort(unsigned short val)
#else
void SV_WriteShort(short val)
#endif
{
    errorIfNotInited("SV_WriteShort");
    lzPutW(val, savefile);
}

#if __JHEXEN__
void SV_WriteLong(unsigned int val)
#else
void SV_WriteLong(long val)
#endif
{
    errorIfNotInited("SV_WriteLong");
    lzPutL(val, savefile);
}

void SV_WriteFloat(float val)
{
    int32_t temp = 0;
    assert(sizeof(val) == 4);
    errorIfNotInited("SV_WriteFloat");
    memcpy(&temp, &val, 4);
    lzPutL(temp, savefile);
}

void SV_Read(void *data, int len)
{
    errorIfNotInited("SV_Read");
#if __JHEXEN__
    memcpy(data, saveptr.b, len);
    saveptr.b += len;
#else
    lzRead(data, len, savefile);
#endif
}

byte SV_ReadByte(void)
{
    errorIfNotInited("SV_ReadByte");
#if __JHEXEN__
    return (*saveptr.b++);
#else
    return lzGetC(savefile);
#endif
}

short SV_ReadShort(void)
{
    errorIfNotInited("SV_ReadShort");
#if __JHEXEN__
    return (SHORT(*saveptr.w++));
#else
    return lzGetW(savefile);
#endif
}

long SV_ReadLong(void)
{
    errorIfNotInited("SV_ReadLong");
#if __JHEXEN__
    return (LONG(*saveptr.l++));
#else
    return lzGetL(savefile);
#endif
}

float SV_ReadFloat(void)
{
#if !__JHEXEN__
    float returnValue = 0;
    int32_t val;
#endif
    errorIfNotInited("SV_ReadFloat");
#if __JHEXEN__
    return (FLOAT(*saveptr.f++));
#else
    val = lzGetL(savefile);
    returnValue = 0;
    assert(sizeof(float) == 4);
    memcpy(&returnValue, &val, 4);
    return returnValue;
#endif
}

static void swi8(Writer* w, char i)
{
    if(!w) return;
    SV_WriteByte(i);
}

static void swi16(Writer* w, short i)
{
    if(!w) return;
    SV_WriteShort(i);
}

static void swi32(Writer* w, int i)
{
    if(!w) return;
    SV_WriteLong(i);
}

static void swf(Writer* w, float i)
{
    if(!w) return;
    SV_WriteFloat(i);
}

static void swd(Writer* w, const char* data, int len)
{
    if(!w) return;
    SV_Write(data, len);
}

void SV_MaterialArchive_Write(MaterialArchive* arc)
{
    Writer* svWriter = Writer_NewWithCallbacks(swi8, swi16, swi32, swf, swd);
    MaterialArchive_Write(arc, svWriter);
    Writer_Delete(svWriter);
}

static char sri8(Reader* r)
{
    if(!r) return 0;
    return SV_ReadByte();
}

static short sri16(Reader* r)
{
    if(!r) return 0;
    return SV_ReadShort();
}

static int sri32(Reader* r)
{
    if(!r) return 0;
    return SV_ReadLong();
}

static float srf(Reader* r)
{
    if(!r) return 0;
    return SV_ReadFloat();
}

static void srd(Reader* r, char* data, int len)
{
    if(!r) return;
    SV_Read(data, len);
}

void SV_MaterialArchive_Read(MaterialArchive* arc, int version)
{
    Reader* svReader = Reader_NewWithCallbacks(sri8, sri16, sri32, srf, srd);
    MaterialArchive_Read(arc, version, svReader);
    Reader_Delete(svReader);
}
