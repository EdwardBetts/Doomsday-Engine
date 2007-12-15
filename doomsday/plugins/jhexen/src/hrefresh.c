/**\file
 *\section License
 * License: Raven
 * Online License Link: http://www.dengine.net/raven_license/End_User_License_Hexen_Source_Code.html
 *
 *\author Copyright © 2003-2007 Jaakko Keränen <jaakko.keranen@iki.fi>
 *\author Copyright © 2005-2007 Daniel Swanson <danij@dengine.net>
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

/**
 * hrefresh.h: - jHexen specific.
 */

// HEADER FILES ------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jhexen.h"

#include "f_infine.h"
#include "r_common.h"
#include "p_mapsetup.h"
#include "g_controls.h"
#include "am_map.h"
#include "g_common.h"
#include "hu_menu.h"

// MACROS ------------------------------------------------------------------

#define FMAKERGBA(r,g,b,a) ( (byte)(0xff*r) + ((byte)(0xff*g)<<8) + ((byte)(0xff*b)<<16) + ((byte)(0xff*a)<<24) )

#define viewheight              Get(DD_VIEWWINDOW_HEIGHT)

#define SIZEFACT                4
#define SIZEFACT2               16

// TYPES -------------------------------------------------------------------

// This could hold much more detailed information...
typedef struct textype_s {
    char    name[9];            // Name of the texture.
    int     type;               // Which type?
} textype_t;

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

void    X_Drawer(void);
void    R_SetAllDoomsdayFlags(void);
void    H2_AdvanceDemo(void);
void    MN_DrCenterTextA_CS(char *text, int center_x, int y);

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

extern float lookOffset;
extern int actual_leveltime;

extern float deffontRGB[];

extern boolean dontrender;

// PUBLIC DATA DEFINITIONS -------------------------------------------------

boolean setsizeneeded;

// wipegamestate can be set to -1 to force a wipe on the next draw
gamestate_t wipegamestate = GS_DEMOSCREEN;

// PRIVATE DATA DEFINITIONS ------------------------------------------------

// CODE --------------------------------------------------------------------

void R_Init(void)
{
    // Nothing to do.
}

/**
 * Don't really change anything here, because i might be in the middle of
 * a refresh.  The change will take effect next refresh.
 */
void R_SetViewSize(int blocks, int detail)
{
    setsizeneeded = true;
    if(cfg.setblocks != blocks && blocks > 10 && blocks < 13)
    {   // When going fullscreen, force a hud show event (to reset the timer).
        ST_HUDUnHide(HUE_FORCE);
    }
    cfg.setblocks = blocks;
}

void R_DrawMapTitle(void)
{
    float       alpha = 1;
    int         y = 12;
    char       *lname, *lauthor;

    if(!cfg.levelTitle || actual_leveltime > 6 * 35)
        return;

    // Make the text a bit smaller.
    gl.MatrixMode(DGL_MODELVIEW);
    gl.PushMatrix();
    gl.Translatef(160, y, 0);
    gl.Scalef(.75f, .75f, 1);   // Scale to 3/4
    gl.Translatef(-160, -y, 0);

    if(actual_leveltime < 35)
        alpha = actual_leveltime / 35.0f;
    if(actual_leveltime > 5 * 35)
        alpha = 1 - (actual_leveltime - 5 * 35) / 35.0f;

    lname = P_GetMapNiceName();
    lauthor = (char *) DD_GetVariable(DD_MAP_AUTHOR);

    // Use stardard map name if DED didn't define it.
    if(!lname)
        lname = P_GetMapName(gamemap);

    Draw_BeginZoom((1 + cfg.hudScale)/2, 160, y);

    if(lname)
    {
        M_WriteText3(160 - M_StringWidth(lname, hu_font_b) / 2, y, lname,
                    hu_font_b, deffontRGB[0], deffontRGB[1], deffontRGB[2],
                    alpha, false, 0);
        y += 20;
    }

    if(lauthor)
    {
        M_WriteText3(160 - M_StringWidth(lauthor, hu_font_a) / 2, y, lauthor,
                    hu_font_a, .5f, .5f, .5f, alpha, false, 0);
    }

    Draw_EndZoom();

    gl.MatrixMode(DGL_MODELVIEW);
    gl.PopMatrix();
}

void G_Drawer(void)
{
    static boolean viewactivestate = false;
    static boolean menuactivestate = false;
    static int  fullscreenmode = 0;
    static gamestate_t oldgamestate = -1;
    int         py;
    player_t   *vplayer = &players[displayplayer];
    boolean     iscam = (vplayer->plr->flags & DDPF_CAMERA) != 0;   // $democam
    float       x, y, w, h;
    boolean     mapHidesView;

    // $democam: can be set on every frame
    if(cfg.setblocks > 10 || iscam)
    {
        // Full screen.
        R_SetViewWindowTarget(0, 0, 320, 200);
    }
    else
    {
        int w = cfg.setblocks * 32;
        int h = cfg.setblocks * (200 - SBARHEIGHT * cfg.sbarscale / 20) / 10;
        R_SetViewWindowTarget(160 - (w >> 1),
                              (200 - SBARHEIGHT * cfg.sbarscale / 20 - h) >> 1,
                              w, h);
    }

    R_GetViewWindow(&x, &y, &w, &h);
    R_ViewWindow((int) x, (int) y, (int) w, (int) h);

    // Do buffered drawing
    switch(G_GetGameState())
    {
    case GS_LEVEL:
        // Clients should be a little careful about the first frames.
        if(IS_CLIENT && (!Get(DD_GAME_READY) || !Get(DD_GOTFRAME)))
            break;

        // Good luck trying to render the view without a viewpoint...
        if(!vplayer->plr->mo)
            break;

        if(!IS_CLIENT && leveltime < 2)
        {
            // Don't render too early; the first couple of frames
            // might be a bit unstable -- this should be considered
            // a bug, but since there's an easy fix...
            break;
        }

        mapHidesView =
            R_MapObscures(displayplayer, (int) x, (int) y, (int) w, (int) h);

        if(!(MN_CurrentMenuHasBackground() && Hu_MenuAlpha() >= 1) &&
           !mapHidesView)
        {
            boolean special200 = false;
            float viewOffset[2] = {0, 0};
            int viewAngleOffset = ANGLE_MAX * -G_GetLookOffset(displayplayer);

            R_HandleSectorSpecials();
            // Set flags for the renderer.
            if(IS_CLIENT)
            {
                // Server updates mobj flags in NetSv_Ticker.
                R_SetAllDoomsdayFlags();
            }
            GL_SetFilter(vplayer->plr->filter); // $democam
            // Check for the sector special 200: use sky2.
            // I wonder where this is used?
            if(P_ToXSectorOfSubsector(vplayer->plr->mo->subsector)->special == 200)
            {
                special200 = true;
                Rend_SkyParams(0, DD_DISABLE, 0);
                Rend_SkyParams(1, DD_ENABLE, 0);
            }
            // How about a bit of quake?
            if(localQuakeHappening[displayplayer] && !paused)
            {
                int     intensity = localQuakeHappening[displayplayer];

                viewOffset[0] = (float) ((M_Random() % (intensity << 2)) -
                                    (intensity << 1));
                viewOffset[1] = (float) ((M_Random() % (intensity << 2)) -
                                    (intensity << 1));
            }
            DD_SetVariable(DD_VIEWX_OFFSET, &viewOffset[0]);
            DD_SetVariable(DD_VIEWY_OFFSET, &viewOffset[1]);

            // The view angle offset.
            DD_SetVariable(DD_VIEWANGLE_OFFSET, &viewAngleOffset);
            // Render the view.
            if(!dontrender)
            {
                R_RenderPlayerView(vplayer->plr);
            }

            if(special200)
            {
                Rend_SkyParams(0, DD_ENABLE, 0);
                Rend_SkyParams(1, DD_DISABLE, 0);
            }

            if(!iscam)
                X_Drawer(); // Draw the crosshair.
        }

        // Draw the automap.
        AM_Drawer(displayplayer);
        break;

    default:
        break;
    }

    menuactivestate = Hu_MenuIsActive();
    viewactivestate = viewactive;
    oldgamestate = wipegamestate = G_GetGameState();

    if(paused && !fi_active)
    {
        //if(AM_IsMapActive(displayplayer))
            py = 4;
        //else
        //    py = 4; // in jDOOM this is viewwindowy + 4

        GL_DrawPatch(160, py, W_GetNumForName("PAUSED"));
    }
}

void G_Drawer2(void)
{
    // Do buffered drawing
    switch(G_GetGameState())
    {
    case GS_LEVEL:
        if(IS_CLIENT && (!Get(DD_GAME_READY) || !Get(DD_GOTFRAME)))
            break;

        if(!IS_CLIENT && leveltime < 2)
        {
            // Don't render too early; the first couple of frames
            // might be a bit unstable -- this should be considered
            // a bug, but since there's an easy fix...
            break;
        }

        // These various HUD's will be drawn unless Doomsday advises not to
        if(DD_GetInteger(DD_GAME_DRAW_HUD_HINT))
        {
            // Draw HUD displays only visible when the automap is open.
            if(AM_IsMapActive(displayplayer))
                HU_DrawMapCounters();

            // Level information is shown for a few seconds in the
            // beginning of a level.
            R_DrawMapTitle();

            // DJS - Do we need to render a full status bar at this point?
            if (!(AM_IsMapActive(displayplayer) && cfg.automapHudDisplay == 0 ))
            {
                player_t   *vplayer = &players[displayplayer];
                boolean     iscam = (vplayer->plr->flags & DDPF_CAMERA) != 0; // $democam

                if(!iscam)
                {
                    if(true == (viewheight == 200))
                    {
                        // Fullscreen. Which mode?
                        ST_Drawer(cfg.setblocks - 10, true);    // $democam
                    }
                    else
                    {
                        ST_Drawer(0 , true);    // $democam
                    }
                }
            }

            HU_Drawer();
        }
        break;

    case GS_INTERMISSION:
        IN_Drawer();
        break;

    case GS_WAITING:
        GL_DrawRawScreen(W_GetNumForName("TITLE"), 0, 0);
        gl.Color3f(1, 1, 1);
        MN_DrCenterTextA_CS("WAITING... PRESS ESC FOR MENU", 160, 188);
        break;

    default:
        break;
    }

    // InFine is drawn whenever active.
    FI_Drawer();

    // The menu is drawn whenever active.
    Hu_MenuDrawer();
}

int R_GetFilterColor(int filter)
{
    // We have to choose the right color and alpha.
    if(filter >= STARTREDPALS && filter < STARTREDPALS + NUMREDPALS)
        // Red?
        return FMAKERGBA(1, 0, 0, filter / 8.0);    // Full red with filter 8.
    else if(filter >= STARTBONUSPALS && filter < STARTBONUSPALS + NUMBONUSPALS)
        // Light Yellow?
        return FMAKERGBA(1, 1, .5, (filter - STARTBONUSPALS + 1) / 16.0);
    else if(filter >= STARTPOISONPALS &&
            filter < STARTPOISONPALS + NUMPOISONPALS)
        // Green?
        return FMAKERGBA(0, 1, 0, (filter - STARTPOISONPALS + 1) / 16.0);
    else if(filter >= STARTSCOURGEPAL)
        // Orange?
        return FMAKERGBA(1, .5, 0, (STARTSCOURGEPAL + 3 - filter) / 6.0);
    else if(filter >= STARTHOLYPAL)
        // White?
        return FMAKERGBA(1, 1, 1, (STARTHOLYPAL + 3 - filter) / 6.0);
    else if(filter == STARTICEPAL)
        // Light blue?
        return FMAKERGBA(.5f, .5f, 1, .4f);
    else if(filter)
        Con_Error("R_GetFilterColor: Strange filter number: %d.\n", filter);
    return 0;
}

void R_SetFilter(int filter)
{
    GL_SetFilter(R_GetFilterColor(filter));
}

void H2_EndFrame(void)
{
    SN_UpdateActiveSequences();
}

/**
 * Updates ddflags of all visible mobjs (in sectorlinks).
 * Not strictly necessary (in single player games at least) but here
 * we tell the engine about light-emitting objects, special effects,
 * object properties (solid, local, low/nograv, etc.), color translation
 * and other interesting little details.
 */
void R_SetAllDoomsdayFlags(void)
{
    uint        i;
    int         Class;
    mobj_t     *mo;

    // Only visible things are in the sector thinglists, so this is good.
    for(i = 0; i < numsectors; ++i)
        for(mo = P_GetPtr(DMU_SECTOR, i, DMT_MOBJS); mo; mo = mo->snext)
        {
            if(IS_CLIENT && mo->ddflags & DDMF_REMOTE)
                continue;

            // Reset the flags for a new frame.
            mo->ddflags &= DDMF_CLEAR_MASK;

            if(mo->flags & MF_LOCAL)
                mo->ddflags |= DDMF_LOCAL;
            if(mo->flags & MF_SOLID)
                mo->ddflags |= DDMF_SOLID;
            if(mo->flags & MF_MISSILE)
                mo->ddflags |= DDMF_MISSILE;
            if(mo->flags2 & MF2_FLY)
                mo->ddflags |= DDMF_FLY | DDMF_NOGRAVITY;
            if(mo->flags2 & MF2_FLOATBOB)
                mo->ddflags |= DDMF_BOB | DDMF_NOGRAVITY;
            if(mo->flags2 & MF2_LOGRAV)
                mo->ddflags |= DDMF_LOWGRAVITY;
            if(mo->flags & MF_NOGRAVITY /* || mo->flags2 & MF2_FLY */ )
                mo->ddflags |= DDMF_NOGRAVITY;

            // $democam: cameramen are invisible
            if(P_IsCamera(mo))
                mo->ddflags |= DDMF_DONTDRAW;

            // Choose which ddflags to set.
            if(mo->flags2 & MF2_DONTDRAW)
            {
                mo->ddflags |= DDMF_DONTDRAW;
                continue;       // No point in checking the other flags.
            }

            if((mo->flags & MF_BRIGHTSHADOW) == MF_BRIGHTSHADOW)
                mo->ddflags |= DDMF_BRIGHTSHADOW;
            else
            {
                if(mo->flags & MF_SHADOW)
                    mo->ddflags |= DDMF_SHADOW;
                if(mo->flags & MF_ALTSHADOW ||
                   (cfg.translucentIceCorpse && mo->flags & MF_ICECORPSE))
                    mo->ddflags |= DDMF_ALTSHADOW;
            }

            if((mo->flags & MF_VIEWALIGN && !(mo->flags & MF_MISSILE)) ||
               mo->flags & MF_FLOAT || (mo->flags & MF_MISSILE &&
                                        !(mo->flags & MF_VIEWALIGN)))
                mo->ddflags |= DDMF_VIEWALIGN;

            mo->ddflags |= mo->flags & MF_TRANSLATION;

            // Which translation table to use?
            if(mo->flags & MF_TRANSLATION)
            {
                if(mo->player)
                {
                    Class = mo->player->class;
                }
                else
                {
                    Class = mo->special1;
                }
                if(Class > 2)
                {
                    Class = 0;
                }
                // The last two bits.
                mo->ddflags |= Class << DDMF_CLASSTRSHIFT;
            }

            // An offset for the light emitted by this object.
            /*          Class = MobjLightOffsets[mo->type];
               if(Class < 0) Class = 8-Class;
               // Class must now be in range 0-15.
               mo->ddflags |= Class << DDMF_LIGHTOFFSETSHIFT; */

            // The Mage's ice shards need to be a bit smaller.
            // This'll make them half the normal size.
            if(mo->type == MT_SHARDFX1)
                mo->ddflags |= 2 << DDMF_LIGHTSCALESHIFT;
        }
}
