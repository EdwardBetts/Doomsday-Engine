/**\file
 *\section License
 * License: GPL
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright © 2003-2007 Jaakko Keränen <jaakko.keranen@iki.fi>
 *\author Copyright © 2005-2007 Daniel Swanson <danij@dengine.net>
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

/*
 * m_misc.h: Miscellanous Routines
 */

#ifndef __DOOMSDAY_MISCELLAN_H__
#define __DOOMSDAY_MISCELLAN_H__

#include "dd_types.h"
#include "sys_file.h"

#define MAX_READ    8192
#define ISSPACE(c)  ((c) == 0 || (c) == ' ' || (c) == '\t' || (c) == '\n' ||\
                     (c) == '\r')

extern int      rndindex;

// Memory.
void           *M_Malloc(size_t size);
void           *M_Calloc(size_t size);
void           *M_Realloc(void *ptr, size_t size);
void            M_Free(void *ptr);

// File system routines.
void            M_ResetFileIDs(void);
boolean         M_CheckFileID(const char *path);
size_t          M_ReadFile(char const *name, byte **buffer);
size_t          M_ReadFileCLib(char const *name, byte **buffer);
boolean         M_WriteFile(char const *name, void *source, size_t length);
void            M_ExtractFileBase(const char *path, char *dest);
void            M_ExtractFileBase2(const char *path, char *dest,
                                   int max, int ignore);
void            M_GetFileExt(const char *path, char *ext);
void            M_ReplaceFileExt(char *path, char *newext);
boolean         M_CheckPath(char *path);
int             M_FileExists(const char *file);
void            M_TranslatePath(const char *path, char *translated);
void            M_PrependBasePath(const char *path, char *newpath);
void            M_RemoveBasePath(const char *absPath, char *newPath);
const char     *M_Pretty(const char *path);
void            M_ReadLine(char *buffer, int len, DFILE * file);

// Bounding boxes.
void            M_ClearBox(fixed_t *box);
void            M_AddToBox(fixed_t *box, fixed_t x, fixed_t y);
float           M_BoundingBoxDiff(const float in[4], const float out[4]);
void            M_JoinBoxes(float box[4], const float other[4]);

// Text utilities.
char           *M_SkipWhite(char *str);
char           *M_FindWhite(char *str);
char           *M_SkipLine(char *str);
void            M_WriteCommented(FILE * file, const char *text);
void            M_WriteTextEsc(FILE * file, char *text);
boolean         M_IsComment(char *text);
boolean         M_IsStringValidInt(const char *str);
boolean         M_IsStringValidByte(const char *str);
boolean         M_IsStringValidFloat(const char *str);
char           *M_LimitedStrCat(const char *str, unsigned int maxWidth,
                                char separator, char *buf,
                                unsigned int bufLength);
char           *M_StrCatQuoted(char *dest, char *src);

// Random numbers.
byte            M_Random(void);
float           M_FRandom(void);
void            M_ClearRandom(void);

// Math routines.
float           M_ApproxDistancef(float dx, float dy);
float           M_ApproxDistance3(const float delta[3]);
float           M_ApproxDistance3f(float dx, float dy, float dz);
float           M_PointLineDistance(const float *a, const float *b, const float *c);
float           M_PointUnitLineDistance(const float *a, const float *b, const float *c);
float           M_Normalize(float *a);
float           M_Distance(const float *a, const float *b);
void            M_Scale(float *dest, const float *a, float scale);
float           M_DotProduct(const float *a, const float *b);
void            M_CrossProduct(const float *a, const float *b, float *out);
void            M_PointCrossProduct(const float *v1, const float *v2,
                                    const float *v3, float *out);
float           M_TriangleArea(const float *v1, const float *v2,
                               const float *v3);
void            M_RotateVector(float vec[3], float degYaw, float degPitch);
void            M_ProjectPointOnLine(const float *point, const float *linepoint,
                                     const float *delta, float gap,
                                     float *result);
void            M_ProjectViewRelativeLine2D(const float center[2],
                                            boolean alignToViewPlane,
                                            float width, float offset,
                                            float start[2], float end[2]);
int             M_CeilPow2(int num);
int             M_FloorPow2(int num);
int             M_RoundPow2(int num);
int             M_WeightPow2(int num, float weight);
float           M_CycleIntoRange(float value, float length);
double          M_SlopeToAngle(double dx, double dy);
double          M_Length(double x, double y);
uint            M_CRC32(byte *data, uint length);

// Time utilities.
boolean         M_RunTrigger(trigger_t* trigger, timespan_t advanceTime);
boolean         M_CheckTrigger(const trigger_t* trigger, timespan_t advanceTime);

// Other utilities.
int             M_ScreenShot(const char *filename, int bits);

#endif
