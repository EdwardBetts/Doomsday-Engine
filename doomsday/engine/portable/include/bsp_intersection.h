/**
 * @file bsp_intersection.h
 * BSP Builder Intersections. @ingroup map
 *
 * Based on glBSP 2.24 (in turn, based on BSP 2.3), which is hosted on
 * SourceForge: http://sourceforge.net/projects/glbsp/
 *
 * @authors Copyright © 2007-2012 Daniel Swanson <danij@dengine.net>
 * @authors Copyright © 2000-2007 Andrew Apted <ajapted@gmail.com>
 * @authors Copyright © 1998-2000 Colin Reed <cph@moria.org.uk>
 * @authors Copyright © 1998-2000 Lee Killough <killough@rsn.hp.com>
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

#ifndef LIBDENG_MAP_BSP_INTERSECTION
#define LIBDENG_MAP_BSP_INTERSECTION

#include "bsp_edge.h"

struct hplanebuildinfo_s;
struct superblock_s;

typedef struct hplanebuildinfo_s {
    LineDef* lineDef; // Not NULL if partition originated from a linedef.
    LineDef* sourceLineDef;

    double pSX, pSY;
    double pDX, pDY;
    double pPara, pPerp;
    double pLength;
} HPlaneBuildInfo;

typedef struct hplaneintercept_s HPlaneIntercept;

HPlaneIntercept* HPlaneIntercept_Next(HPlaneIntercept* intersection);

HPlaneIntercept* HPlaneIntercept_Prev(HPlaneIntercept* intersection);

void* HPlaneIntercept_UserData(HPlaneIntercept* intersection);

/**
 * HPlane instance. Created with HPlane_New().
 */
typedef struct hplane_s HPlane;

/**
 * Create a new HPlane.
 */
HPlane* HPlane_New(void);

const double* HPlane_Origin(HPlane* hPlane);
double HPlane_X(HPlane* hPlane);
double HPlane_Y(HPlane* hPlane);

HPlane* HPlane_SetOrigin(HPlane* hPlane, double const origin[2]);
HPlane* HPlane_SetXY(HPlane* hPlane, double x, double y);
HPlane* HPlane_SetX(HPlane* hPlane, double x);
HPlane* HPlane_SetY(HPlane* hPlane, double y);

const double* HPlane_Angle(HPlane* hPlane);
double HPlane_DX(HPlane* hPlane);
double HPlane_DY(HPlane* hPlane);

HPlane* HPlane_SetAngle(HPlane* hPlane, double const angle[2]);
HPlane* HPlane_SetDXY(HPlane* hPlane, double x, double y);
HPlane* HPlane_SetDX(HPlane* hPlane, double dx);
HPlane* HPlane_SetDY(HPlane* hPlane, double dy);

HPlaneBuildInfo* HPlane_BuildInfo(HPlane* hPlane);

/**
 * Destroy a HPlane.
 */
void HPlane_Delete(HPlane* hPlane);

/**
 * Empty all intersections from the specified HPlane.
 */
void HPlane_Clear(HPlane* hPlane);

/**
 * Insert a point at the given intersection into the intersection list.
 *
 * @todo Ownership of the user data should NOT be passed to this object.
 *
 * @param userData  Ownership passes to HPlane.
 */
HPlaneIntercept* HPlane_NewIntercept2(HPlane* hPlane, double distance, void* userData);
HPlaneIntercept* HPlane_NewIntercept(HPlane* hPlane, double distance/*, userData=NULL*/);

int HPlane_IterateIntercepts2(HPlane* bi, int (*callback)(HPlaneIntercept*, void*), void* parameters);
int HPlane_IterateIntercepts(HPlane* bi, int (*callback)(HPlaneIntercept*, void*)/*, parameters=NULL*/);

#if _DEBUG
void HPlaneIntercept_Print(HPlane* hPlane);
#endif

void BSP_InitHPlaneInterceptAllocator(void);
void BSP_ShutdownHPlaneInterceptAllocator(void);

/**
 * @todo the following functions do not belong in this module.
 */

void Bsp_MergeIntersections(HPlane* intersections);
void Bsp_BuildHEdgesAtIntersectionGaps(HPlane* hPlane,
    struct superblock_s* rightList, struct superblock_s* leftList);

#endif /// LIBDENG_MAP_BSP_INTERSECTION
