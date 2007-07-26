/**\file
 *\section License
 * License: Raven
 * Online License Link: http://www.dengine.net/raven_license/End_User_License_Hexen_Source_Code.html
 *
 *\author Copyright © 2003-2007 Jaakko Keränen <skyjake@dengine.net>
 *\author Copyright © 2005-2006 Daniel Swanson <danij@dengine.net>
 *\author Copyright © 1999 Activision
 *
 * This program is covered by the HERETIC / HEXEN (LIMITED USE) source
 * code license; you can redistribute it and/or modify it under the terms
 * of the HERETIC / HEXEN source code license as published by Activision.
 *
 * THIS MATERIAL IS NOT MADE OR SUPPORTED BY ACTIVISION.
 *
 * WARRANTY INFORMATION.
 * This program is provided as is. Activision and it's affiliates make no
 * warranties of any kind, whether oral or written , express or implied,
 * including any warranty of merchantability, fitness for a particular
 * purpose or non-infringement, and no other representations or claims of
 * any kind shall be binding on or obligate Activision or it's affiliates.
 *
 * LICENSE CONDITIONS.
 * You shall not:
 *
 * 1) Exploit this Program or any of its parts commercially.
 * 2) Use this Program, or permit use of this Program, on more than one
 *    computer, computer terminal, or workstation at the same time.
 * 3) Make copies of this Program or any part thereof, or make copies of
 *    the materials accompanying this Program.
 * 4) Use the program, or permit use of this Program, in a network,
 *    multi-user arrangement or remote access arrangement, including any
 *    online use, except as otherwise explicitly provided by this Program.
 * 5) Sell, rent, lease or license any copies of this Program, without
 *    the express prior written consent of Activision.
 * 6) Remove, disable or circumvent any proprietary notices or labels
 *    contained on or within the Program.
 *
 * You should have received a copy of the HERETIC / HEXEN source code
 * license along with this program (Ravenlic.txt); if not:
 * http://www.ravensoft.com/
 */

// HEADER FILES ------------------------------------------------------------

#include "jhexen.h"

#include "p_inventory.h"
#include "p_player.h"
#include "p_map.h"
#include "p_mapsetup.h"
#include "p_mapspec.h"

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

static boolean CheckedLockedDoor(mobj_t *mo, byte lock);

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

int    *TerrainTypes = 0;
struct terraindef_s {
    char   *name;
    int     type;
} TerrainTypeDefs[] =
{
    {"X_005", FLOOR_WATER},
    {"X_001", FLOOR_LAVA},
    {"X_009", FLOOR_SLUDGE},
    {"F_033", FLOOR_ICE},
    {"END", -1}
};

// PRIVATE DATA DEFINITIONS ------------------------------------------------

mobj_t  LavaInflictor;

// CODE --------------------------------------------------------------------

void P_InitLava(void)
{
    memset(&LavaInflictor, 0, sizeof(mobj_t));

    LavaInflictor.type = MT_CIRCLEFLAME;
    LavaInflictor.flags2 = MF2_FIREDAMAGE | MF2_NODMGTHRUST;
}

void P_InitTerrainTypes(void)
{
    int     i;
    int     lump;
    int     size = Get(DD_NUMLUMPS) * sizeof(int);

    // Free if there already is memory allocated.
    if(TerrainTypes)
        Z_Free(TerrainTypes);

    TerrainTypes = Z_Malloc(size, PU_STATIC, 0);
    memset(TerrainTypes, 0, size);

    for(i = 0; TerrainTypeDefs[i].type != -1; ++i)
    {
        lump = W_CheckNumForName(TerrainTypeDefs[i].name);
        if(lump != -1)
        {
            TerrainTypes[lump] = TerrainTypeDefs[i].type;
        }
    }
}

/**
 * Return the terrain type of the specified flat.
 *
 * @param flatlumpnum   The flat lump number to check.
 */
int P_FlatToTerrainType(int flatlumpnum)
{
    if(flatlumpnum != -1)
        return TerrainTypes[flatlumpnum];
    else
        return FLOOR_SOLID;
}

/**
 * Returns the terrain type of the specified sector, plane.
 *
 * @param sec       The sector to check.
 * @param plane     The plane id to check.
 */
int P_GetTerrainType(sector_t* sec, int plane)
{
    int flatnum = P_GetIntp(sec, (plane? DMU_CEILING_TEXTURE : DMU_FLOOR_TEXTURE));

    if(flatnum != -1)
        return TerrainTypes[flatnum];
    else
        return FLOOR_SOLID;
}

boolean EV_SectorSoundChange(byte *args)
{
    boolean     rtn = false;
    sector_t   *sec = NULL;
    iterlist_t *list;

    if(!args[0])
        return false;

    list = P_GetSectorIterListForTag((int) args[0], false);
    if(!list)
        return rtn;

    P_IterListResetIterator(list, true);
    while((sec = P_IterListIterator(list)) != NULL)
    {
        P_XSector(sec)->seqType = args[1];
        rtn = true;
    }

    return rtn;
}

static boolean CheckedLockedDoor(mobj_t *mo, byte lock)
{
    extern int  TextKeyMessages[11];
    char        LockedBuffer[80];

    if(!mo->player)
        return false;

    if(!lock)
        return true;

    if(!(mo->player->keys & (1 << (lock - 1))))
    {
        sprintf(LockedBuffer, "YOU NEED THE %s\n",
                GET_TXT(TextKeyMessages[lock - 1]));

        P_SetMessage(mo->player, LockedBuffer, false);
        S_StartSound(SFX_DOOR_LOCKED, mo);
        return false;
    }

    return true;
}

boolean EV_LineSearchForPuzzleItem(line_t *line, byte *args, mobj_t *mo)
{
    artitype_e  arti;

    if(!mo || !mo->player || !line)
        return false;

    arti = arti_firstpuzzitem + P_XLine(line)->arg1;

    if(arti < arti_firstpuzzitem)
        return false;

    // Search player's inventory for puzzle items
    return P_InventoryUseArtifact(mo->player, arti);
}

boolean P_ExecuteLineSpecial(int special, byte *args, line_t *line, int side,
                             mobj_t *mo)
{
    boolean     buttonSuccess;

    buttonSuccess = false;
    switch(special)
    {
    case 1: // Poly Start Line
        break;

    case 2: // Poly Rotate Left
        buttonSuccess = EV_RotatePoly(line, args, 1, false);
        break;

    case 3: // Poly Rotate Right
        buttonSuccess = EV_RotatePoly(line, args, -1, false);
        break;

    case 4: // Poly Move
        buttonSuccess = EV_MovePoly(line, args, false, false);
        break;

    case 5: // Poly Explicit Line:  Only used in initialization
        break;

    case 6: // Poly Move Times 8
        buttonSuccess = EV_MovePoly(line, args, true, false);
        break;

    case 7: // Poly Door Swing
        buttonSuccess = EV_OpenPolyDoor(line, args, PODOOR_SWING);
        break;

    case 8: // Poly Door Slide
        buttonSuccess = EV_OpenPolyDoor(line, args, PODOOR_SLIDE);
        break;

    case 10: // Door Close
        buttonSuccess = EV_DoDoor(line, args, close);
        break;

    case 11: // Door Open
        if(!args[0])
        {
            buttonSuccess = EV_VerticalDoor(line, mo);
        }
        else
        {
            buttonSuccess = EV_DoDoor(line, args, open);
        }
        break;

    case 12: // Door Raise
        if(!args[0])
        {
            buttonSuccess = EV_VerticalDoor(line, mo);
        }
        else
        {
            buttonSuccess = EV_DoDoor(line, args, normal);
        }
        break;

    case 13: // Door Locked_Raise
        if(CheckedLockedDoor(mo, args[3]))
        {
            if(!args[0])
            {
                buttonSuccess = EV_VerticalDoor(line, mo);
            }
            else
            {
                buttonSuccess = EV_DoDoor(line, args, normal);
            }
        }
        break;

    case 20: // Floor Lower by Value
        buttonSuccess = EV_DoFloor(line, args, FLEV_LOWERFLOORBYVALUE);
        break;

    case 21: // Floor Lower to Lowest
        buttonSuccess = EV_DoFloor(line, args, FLEV_LOWERFLOORTOLOWEST);
        break;

    case 22: // Floor Lower to Nearest
        buttonSuccess = EV_DoFloor(line, args, FLEV_LOWERFLOOR);
        break;

    case 23: // Floor Raise by Value
        buttonSuccess = EV_DoFloor(line, args, FLEV_RAISEFLOORBYVALUE);
        break;

    case 24: // Floor Raise to Highest
        buttonSuccess = EV_DoFloor(line, args, FLEV_RAISEFLOOR);
        break;

    case 25: // Floor Raise to Nearest
        buttonSuccess = EV_DoFloor(line, args, FLEV_RAISEFLOORTONEAREST);
        break;

    case 26: // Stairs Build Down Normal
        buttonSuccess = EV_BuildStairs(line, args, -1, STAIRS_NORMAL);
        break;

    case 27: // Build Stairs Up Normal
        buttonSuccess = EV_BuildStairs(line, args, 1, STAIRS_NORMAL);
        break;

    case 28: // Floor Raise and Crush
        buttonSuccess = EV_DoFloor(line, args, FLEV_RAISEFLOORCRUSH);
        break;

    case 29: // Build Pillar (no crushing)
        buttonSuccess = EV_BuildPillar(line, args, false);
        break;

    case 30: // Open Pillar
        buttonSuccess = EV_OpenPillar(line, args);
        break;

    case 31: // Stairs Build Down Sync
        buttonSuccess = EV_BuildStairs(line, args, -1, STAIRS_SYNC);
        break;

    case 32: // Build Stairs Up Sync
        buttonSuccess = EV_BuildStairs(line, args, 1, STAIRS_SYNC);
        break;

    case 35: // Raise Floor by Value Times 8
        buttonSuccess = EV_DoFloor(line, args, FLEV_RAISEBYVALUETIMES8);
        break;

    case 36: // Lower Floor by Value Times 8
        buttonSuccess = EV_DoFloor(line, args, FLEV_LOWERBYVALUETIMES8);
        break;

    case 40: // Ceiling Lower by Value
        buttonSuccess = EV_DoCeiling(line, args, lowerByValue);
        break;

    case 41: // Ceiling Raise by Value
        buttonSuccess = EV_DoCeiling(line, args, raiseByValue);
        break;

    case 42: // Ceiling Crush and Raise
        buttonSuccess = EV_DoCeiling(line, args, crushAndRaise);
        break;

    case 43: // Ceiling Lower and Crush
        buttonSuccess = EV_DoCeiling(line, args, lowerAndCrush);
        break;

    case 44: // Ceiling Crush Stop
        buttonSuccess = EV_CeilingCrushStop(line, args);
        break;

    case 45: // Ceiling Crush Raise and Stay
        buttonSuccess = EV_DoCeiling(line, args, crushRaiseAndStay);
        break;

    case 46: // Floor Crush Stop
        buttonSuccess = EV_FloorCrushStop(line, args);
        break;

    case 60: // Plat Perpetual Raise
        buttonSuccess = EV_DoPlat(line, args, perpetualRaise, 0);
        break;

    case 61: // Plat Stop
        EV_StopPlat(line, args);
        break;

    case 62: // Plat Down-Wait-Up-Stay
        buttonSuccess = EV_DoPlat(line, args, downWaitUpStay, 0);
        break;

    case 63: // Plat Down-by-Value*8-Wait-Up-Stay
        buttonSuccess = EV_DoPlat(line, args, downByValueWaitUpStay, 0);
        break;

    case 64: // Plat Up-Wait-Down-Stay
        buttonSuccess = EV_DoPlat(line, args, upWaitDownStay, 0);
        break;

    case 65: // Plat Up-by-Value*8-Wait-Down-Stay
        buttonSuccess = EV_DoPlat(line, args, upByValueWaitDownStay, 0);
        break;

    case 66: // Floor Lower Instant * 8
        buttonSuccess = EV_DoFloor(line, args, FLEV_LOWERTIMES8INSTANT);
        break;

    case 67: // Floor Raise Instant * 8
        buttonSuccess = EV_DoFloor(line, args, FLEV_RAISETIMES8INSTANT);
        break;

    case 68: // Floor Move to Value * 8
        buttonSuccess = EV_DoFloor(line, args, FLEV_MOVETOVALUETIMES8);
        break;

    case 69: // Ceiling Move to Value * 8
        buttonSuccess = EV_DoCeiling(line, args, moveToValueTimes8);
        break;

    case 70: // Teleport
        if(side == 0)
        {   // Only teleport when crossing the front side of a line
            buttonSuccess = EV_Teleport(args[0], mo, true);
        }
        break;

    case 71: // Teleport, no fog
        if(side == 0)
        {   // Only teleport when crossing the front side of a line
            buttonSuccess = EV_Teleport(args[0], mo, false);
        }
        break;

    case 72: // Thrust Mobj
        if(!side) // Only thrust on side 0
        {
            P_ThrustMobj(mo, args[0] * (ANGLE_90 / 64), args[1] << FRACBITS);
            buttonSuccess = 1;
        }
        break;

    case 73: // Damage Mobj
        if(args[0])
        {
            P_DamageMobj(mo, NULL, NULL, args[0]);
        }
        else
        {   // If arg1 is zero, then guarantee a kill
            P_DamageMobj(mo, NULL, NULL, 10000);
        }
        buttonSuccess = 1;
        break;

    case 74: // Teleport_NewMap
        if(side == 0) // Only teleport when crossing the front side of a line
        {
            // Players must be alive to teleport
            if(!(mo && mo->player && mo->player->playerstate == PST_DEAD))
            {
                G_LeaveLevel(args[0], args[1], false);
                buttonSuccess = true;
            }
        }
        break;

    case 75: // Teleport_EndGame
        if(side == 0)  // Only teleport when crossing the front side of a line
        {
            // Players must be alive to teleport
            if(!(mo && mo->player && mo->player->playerstate == PST_DEAD))
            {
                buttonSuccess = true;
                if(deathmatch)
                {
                    // Winning in deathmatch just goes back to map 1
                    G_LeaveLevel(1, 0, false);
                }
                else
                {
                    // Passing -1, -1 to G_LeaveLevel() starts the Finale
                    G_LeaveLevel(-1, -1, false);
                }
            }
        }
        break;

    case 80: // ACS_Execute
        buttonSuccess = P_StartACS(args[0], args[1], &args[2], mo, line, side);
        break;

    case 81: // ACS_Suspend
        buttonSuccess = P_SuspendACS(args[0], args[1]);
        break;

    case 82: // ACS_Terminate
        buttonSuccess = P_TerminateACS(args[0], args[1]);
        break;

    case 83: // ACS_LockedExecute
        buttonSuccess = P_StartLockedACS(line, args, mo, side);
        break;

    case 90: // Poly Rotate Left Override
        buttonSuccess = EV_RotatePoly(line, args, 1, true);
        break;

    case 91: // Poly Rotate Right Override
        buttonSuccess = EV_RotatePoly(line, args, -1, true);
        break;

    case 92: // Poly Move Override
        buttonSuccess = EV_MovePoly(line, args, false, true);
        break;

    case 93: // Poly Move Times 8 Override
        buttonSuccess = EV_MovePoly(line, args, true, true);
        break;

    case 94: // Build Pillar Crush
        buttonSuccess = EV_BuildPillar(line, args, true);
        break;

    case 95: // Lower Floor and Ceiling
        buttonSuccess = EV_DoFloorAndCeiling(line, args, false);
        break;

    case 96: // Raise Floor and Ceiling
        buttonSuccess = EV_DoFloorAndCeiling(line, args, true);
        break;

    case 109: // Force Lightning
        buttonSuccess = true;
        P_ForceLightning();
        break;

    case 110: // Light Raise by Value
        buttonSuccess = EV_SpawnLight(line, args, LITE_RAISEBYVALUE);
        break;

    case 111: // Light Lower by Value
        buttonSuccess = EV_SpawnLight(line, args, LITE_LOWERBYVALUE);
        break;

    case 112: // Light Change to Value
        buttonSuccess = EV_SpawnLight(line, args, LITE_CHANGETOVALUE);
        break;

    case 113: // Light Fade
        buttonSuccess = EV_SpawnLight(line, args, LITE_FADE);
        break;

    case 114: // Light Glow
        buttonSuccess = EV_SpawnLight(line, args, LITE_GLOW);
        break;

    case 115: // Light Flicker
        buttonSuccess = EV_SpawnLight(line, args, LITE_FLICKER);
        break;

    case 116: // Light Strobe
        buttonSuccess = EV_SpawnLight(line, args, LITE_STROBE);
        break;

    case 120: // Quake Tremor
        buttonSuccess = A_LocalQuake(args, mo);
        break;

    case 129: // UsePuzzleItem
        buttonSuccess = EV_LineSearchForPuzzleItem(line, args, mo);
        break;

    case 130: // Thing_Activate
        buttonSuccess = EV_ThingActivate(args[0]);
        break;

    case 131: // Thing_Deactivate
        buttonSuccess = EV_ThingDeactivate(args[0]);
        break;

    case 132: // Thing_Remove
        buttonSuccess = EV_ThingRemove(args[0]);
        break;

    case 133: // Thing_Destroy
        buttonSuccess = EV_ThingDestroy(args[0]);
        break;

    case 134: // Thing_Projectile
        buttonSuccess = EV_ThingProjectile(args, 0);
        break;

    case 135: // Thing_Spawn
        buttonSuccess = EV_ThingSpawn(args, 1);
        break;

    case 136: // Thing_ProjectileGravity
        buttonSuccess = EV_ThingProjectile(args, 1);
        break;

    case 137: // Thing_SpawnNoFog
        buttonSuccess = EV_ThingSpawn(args, 0);
        break;

    case 138: // Floor_Waggle
        buttonSuccess =
            EV_StartFloorWaggle(args[0], args[1], args[2], args[3], args[4]);
        break;

    case 140: // Sector_SoundChange
        buttonSuccess = EV_SectorSoundChange(args);
        break;

    default:
        break;
    }

    return buttonSuccess;
}

boolean P_ActivateLine(line_t *line, mobj_t *mo, int side, int activationType)
{
    int         lineActivation;
    boolean     repeat;
    boolean     buttonSuccess;

    lineActivation = GET_SPAC(P_GetIntp(line, DMU_FLAGS));
    if(lineActivation != activationType)
        return false;

    if(!mo->player && !(mo->flags & MF_MISSILE))
    {
        if(lineActivation != SPAC_MCROSS)
        {   // currently, monsters can only activate the MCROSS activation type
            return false;
        }

        if(P_GetIntp(line, DMU_FLAGS) & ML_SECRET)
            return false; // never open secret doors
    }

    repeat = P_GetIntp(line, DMU_FLAGS) & ML_REPEAT_SPECIAL;
    buttonSuccess = false;

    buttonSuccess =
        P_ExecuteLineSpecial(P_XLine(line)->special, &P_XLine(line)->arg1,
                             line, side, mo);
    if(!repeat && buttonSuccess)
    {
        // clear the special on non-retriggerable lines
        P_XLine(line)->special = 0;
    }

    if((lineActivation == SPAC_USE || lineActivation == SPAC_IMPACT) &&
       buttonSuccess)
    {
        P_ChangeSwitchTexture(line, repeat);
    }

    return true;
}

/**
 * Called every tic frame that the player origin is in a special sector.
 */
void P_PlayerInSpecialSector(player_t *player)
{
    sector_t   *sector;
    xsector_t  *xsector;
    static int pushTab[3] = {
        2048 * 5,
        2048 * 10,
        2048 * 25
    };

    sector = P_GetPtrp(player->plr->mo->subsector, DMU_SECTOR);
    xsector = P_XSector(sector);

    if(player->plr->mo->pos[VZ] != P_GetFixedp(sector, DMU_FLOOR_HEIGHT))
        return; // Player is not touching the floor

    switch(xsector->special)
    {
    case 9: // SecretArea
        player->secretcount++;
        xsector->special = 0;
        break;

    case 201:
    case 202:
    case 203: // Scroll_North_xxx
        P_Thrust(player, ANG90, pushTab[xsector->special - 201]);
        break;

    case 204:
    case 205:
    case 206: // Sxcroll_East_xxx
        P_Thrust(player, 0, pushTab[xsector->special - 204]);
        break;

    case 207:
    case 208:
    case 209: // Scroll_South_xxx
        P_Thrust(player, ANG270, pushTab[xsector->special - 207]);
        break;

    case 210:
    case 211:
    case 212: // Scroll_West_xxx
        P_Thrust(player, ANG180, pushTab[xsector->special - 210]);
        break;

    case 213:
    case 214:
    case 215: // Scroll_NorthWest_xxx
        P_Thrust(player, ANG90 + ANG45, pushTab[xsector->special - 213]);
        break;

    case 216:
    case 217:
    case 218: // Scroll_NorthEast_xxx
        P_Thrust(player, ANG45, pushTab[xsector->special - 216]);
        break;

    case 219:
    case 220:
    case 221: // Scroll_SouthEast_xxx
        P_Thrust(player, ANG270 + ANG45, pushTab[xsector->special - 219]);
        break;

    case 222:
    case 223:
    case 224: // Scroll_SouthWest_xxx
        P_Thrust(player, ANG180 + ANG45, pushTab[xsector->special - 222]);
        break;

    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
        // Wind specials are handled in (P_mobj):P_XYMovement
        break;

    case 26: // Stairs_Special1
    case 27: // Stairs_Special2
        // Used in (P_floor):ProcessStairSector
        break;

    case 198: // Lightning Special
    case 199: // Lightning Flash special
    case 200: // Sky2
        // Used in (R_plane):R_Drawplanes
        break;

    default:
        if(IS_CLIENT)
            break;

        Con_Error("P_PlayerInSpecialSector: unknown special %i",
                  xsector->special);
    }
}

void P_PlayerOnSpecialFlat(player_t *player, int floorType)
{
    if(player->plr->mo->pos[VZ]
       > P_GetFixedp(player->plr->mo->subsector, DMU_SECTOR_OF_SUBSECTOR | DMU_FLOOR_HEIGHT))
    {
        return; // Player is not touching the floor
    }

    switch(floorType)
    {
    case FLOOR_LAVA:
        if(!(leveltime & 31))
        {
            P_DamageMobj(player->plr->mo, &LavaInflictor, NULL, 10);
            S_StartSound(SFX_LAVA_SIZZLE, player->plr->mo);
        }
        break;

    default:
        break;
    }
}

void P_UpdateSpecials(void)
{
    button_t *button;

    //  DO BUTTONS
    for(button = buttonlist; button; button = button->next)
    {
        if(button->btimer)
        {
            button->btimer--;
            if(!button->btimer)
            {
                side_t     *sdef = P_GetPtrp(button->line, DMU_SIDE0);
                //sector_t *frontsector = P_GetPtrp(button->line, DMU_FRONT_SECTOR);

                switch(button->where)
                {
                case top:
                    P_SetIntp(sdef, DMU_TOP_TEXTURE, button->btexture);
                    break;

                case middle:
                    P_SetIntp(sdef, DMU_MIDDLE_TEXTURE, button->btexture);
                    break;

                case bottom:
                    P_SetIntp(sdef, DMU_BOTTOM_TEXTURE, button->btexture);
                    break;

                default:
                    Con_Error("P_UpdateSpecials: Unknown sidedef section \"%d\".",
                              button->where);
                }

                button->line = NULL;
                button->where = 0;
                button->btexture = 0;
                button->soundorg = NULL;
            }
        }
    }
}

void P_FreeButtons(void)
{
    button_t *button, *np;

    button = buttonlist;
    while(button != NULL)
    {
        np = button->next;
        free(button);
        button = np;
    }
    buttonlist = NULL;
}

/**
 * After the map has been loaded, scan for specials that spawn thinkers.
 */
void P_SpawnSpecials(void)
{
    uint        i;
    line_t     *line;
    xline_t    *xline;
    iterlist_t *list;
    sector_t   *sec;
    xsector_t  *xsec;

    // Init special SECTORs.
    P_DestroySectorTagLists();
    for(i = 0; i < numsectors; ++i)
    {
        sec = P_ToPtr(DMU_SECTOR, i);
        xsec = P_XSector(sec);

        if(xsec->tag)
        {
           list = P_GetSectorIterListForTag(xsec->tag, true);
           P_AddObjectToIterList(list, sec);
        }

        // Clients do not spawn sector specials.
        if(IS_CLIENT)
            break;

        if(!xsec->special)
            continue;

        switch(xsec->special)
        {
        case 1: // Phased light
            // Hardcoded base, use sector->lightlevel as the index
            P_SpawnPhasedLight(sec, (80.f / 255.0f), -1);
            break;

        case 2:// Phased light sequence start
            P_SpawnLightSequence(sec, 1);
            break;
            // Specials 3 & 4 are used by the phased light sequences
        }
    }

    // Init animating line specials.
    P_EmptyIterList(linespecials);
    P_DestroyLineTagLists();
    for(i = 0; i < numlines; ++i)
    {
        line = P_ToPtr(DMU_LINE, i);
        xline = P_XLine(line);

        switch(xline->special)
        {
        case 100: // Scroll_Texture_Left
        case 101: // Scroll_Texture_Right
        case 102: // Scroll_Texture_Up
        case 103: // Scroll_Texture_Down
            P_AddObjectToIterList(linespecials, line);
            break;

        case 121:               // Line_SetIdentification
            if(xline->arg1)
            {
                list = P_GetLineIterListForTag((int) xline->arg1, true);
                P_AddObjectToIterList(list, line);
            }
            xline->special = 0;
            break;
        }
    }

    //// \fixme Remove fixed limits.
    P_RemoveAllActiveCeilings();  // jff 2/22/98 use killough's scheme
    P_RemoveAllActivePlats();

    P_FreeButtons();
}

void R_HandleSectorSpecials(void)
{
    uint        i;
    int         scrollOffset = leveltime >> 1 & 63;

    for(i = 0; i < numsectors; ++i)
    {
        xsector_t* sect = P_XSector(P_ToPtr(DMU_SECTOR, i));

        switch(sect->special)
        {                       // Handle scrolling flats
        case 201:
        case 202:
        case 203:               // Scroll_North_xxx
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_Y,
                       (63 - scrollOffset) << (sect->special - 201));
            break;

        case 204:
        case 205:
        case 206:               // Scroll_East_xxx
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_X,
                       (63 - scrollOffset) << (sect->special - 204));
            break;

        case 207:
        case 208:
        case 209:               // Scroll_South_xxx
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_Y,
                       scrollOffset << (sect->special - 207));
            break;

        case 210:
        case 211:
        case 212:               // Scroll_West_xxx
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_X,
                       scrollOffset << (sect->special - 210));
            break;

        case 213:
        case 214:
        case 215:               // Scroll_NorthWest_xxx
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_X,
                       scrollOffset << (sect->special - 213));
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_Y,
                       (63 - scrollOffset) << (sect->special - 213));
            break;

        case 216:
        case 217:
        case 218:               // Scroll_NorthEast_xxx
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_X,
                       (63 - scrollOffset) << (sect->special - 216));
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_Y,
                       (63 - scrollOffset) << (sect->special - 216));
            break;

        case 219:
        case 220:
        case 221:               // Scroll_SouthEast_xxx
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_X,
                       (63 - scrollOffset) << (sect->special - 219));
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_Y,
                       scrollOffset << (sect->special - 219));
            break;

        case 222:
        case 223:
        case 224:               // Scroll_SouthWest_xxx
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_X,
                       scrollOffset << (sect->special - 222));
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_Y,
                       scrollOffset << (sect->special - 222));
            break;

        default:
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_X, 0);
            P_SetFloat(DMU_SECTOR, i, DMU_FLOOR_OFFSET_Y, 0);
            break;
        }
    }
}
