/* DE1: $Id$
 * Copyright (C) 2003, 2004 Jaakko Ker�nen <jaakko.keranen@iki.fi>
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
 * along with this program; if not: http://www.opensource.org/
 */

/*
 * gl_main.h: Graphics Subsystem
 */

#ifndef __DOOMSDAY_GRAPHICS_H__
#define __DOOMSDAY_GRAPHICS_H__

#include "con_decl.h"
#include "r_main.h"

typedef enum glfontstyle_e {
    GLFS_NORMAL,
    GLFS_BOLD,
    GLFS_LIGHT,
    NUM_GLFS
} glfontstyle_t;

boolean         GL_IsInited(void);

void            GL_Register(void);
void            GL_Init(void);
void            GL_Shutdown(void);
void            GL_TotalReset(boolean doShutdown,
                              boolean loadLightMaps, boolean loadFlareMaps);
void            GL_Init2DState(void);
void            GL_SwitchTo3DState(boolean push_state);
void            GL_Restore2DState(int step);
void            GL_ProjectionMatrix(void);
void            GL_RuntimeMode(void);
int             GL_ChangeResolution(int w, int h, int bits);
void            GL_Update(int flags);
void            GL_DoUpdate(void);
void            GL_BlendMode(blendmode_t mode);

void            GL_InitRefresh(boolean loadLightMaps, boolean loadFlareMaps);
void            GL_ShutdownRefresh(void);
void            GL_UseFog(int yes);
void            GL_InitVarFont(void);
void            GL_ShutdownVarFont(void);
const char*     GL_ChooseFixedFont();
const char*     GL_ChooseVariableFont(glfontstyle_t style);

// Returns a pointer to a copy of the screen. The pointer must be
// deallocated by the caller.
unsigned char  *GL_GrabScreen(void);

extern int      glScreenWidth, glScreenHeight, glScreenbits;
extern int      numTexUnits;
extern boolean  envModAdd;
extern int      defResX, defResY;
extern float    nearClip, glFarClip;
extern int      viewph, viewpw, viewpx, viewpy;
extern int      r_framecounter;
extern char     hiTexPath[], hiTexPath2[];
extern int      UpdateState;
extern float    vid_gamma, vid_bright, vid_contrast;
extern int      glFontFixed, glFontVariable[NUM_GLFS];
extern int      r_detail;

// Console commands.
D_CMD(UpdateGammaRamp);

#endif
