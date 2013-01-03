/**
 * @file p_scroll.c
 * Common surface material scroll thinker. @ingroup libcommon
 *
 * @authors Copyright &copy; 2003-2012 Jaakko Keränen <jaakko.keranen@iki.fi>
 * @authors Copyright &copy; 2005-2012 Daniel Swanson <danij@dengine.net>
 * @authors Copyright &copy; 1993-1996 by id Software, Inc.
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

#include "common.h"
#include "dmu_lib.h"
#include "p_scroll.h"

void T_Scroll(scroll_t* s)
{
    DENG_ASSERT(s);

    if(FEQUAL(s->offset[0], 0) && FEQUAL(s->offset[1], 0)) return;

    // Side surface(s)?
    if(DMU_GetType(s->dmuObject) == DMU_SIDEDEF)
    {
        SideDef* side = s->dmuObject;

        if(s->elementBits & (1 << SS_TOP))
        {
            P_TranslateSideMaterialOrigin(side, SS_TOP, s->offset);
        }
        if(s->elementBits & (1 << SS_MIDDLE))
        {
            P_TranslateSideMaterialOrigin(side, SS_MIDDLE, s->offset);
        }
        if(s->elementBits & (1 << SS_BOTTOM))
        {
            P_TranslateSideMaterialOrigin(side, SS_BOTTOM, s->offset);
        }
    }
    else // Sector plane-surface(s).
    {
        Sector* sector = s->dmuObject;
        if(s->elementBits & (1 << PLN_FLOOR))
        {
            Plane* plane = P_GetPtrp(sector, DMU_FLOOR_PLANE);
            P_TranslatePlaneMaterialOrigin(plane, s->offset);
        }
        if(s->elementBits & (1 << PLN_CEILING))
        {
            Plane* plane = P_GetPtrp(sector, DMU_CEILING_PLANE);
            P_TranslatePlaneMaterialOrigin(plane, s->offset);
        }
    }
}

static scroll_t* spawnMaterialOriginScroller(void* dmuObject, int elementBits, float offsetXY[2])
{
    scroll_t* scroll;

    // Don't spawn a scroller with an invalid map object reference.
    if(!dmuObject || elementBits <= 0) return NULL;

    // Don't spawn a scroller with a zero-length offset vector.
    if(FEQUAL(offsetXY[0], 0) && FEQUAL(offsetXY[1], 0)) return NULL;

    scroll = Z_Calloc(sizeof(*scroll), PU_MAP, 0);
    scroll->thinker.function = T_Scroll;
    Thinker_Add(&scroll->thinker);

    scroll->dmuObject = dmuObject;
    scroll->elementBits = elementBits;
    scroll->offset[0] = offsetXY[0];
    scroll->offset[1] = offsetXY[1];

    return scroll;
}

scroll_t* P_SpawnSideMaterialOriginScroller(SideDef* side, short special)
{
    int elementBits;
    float offset[2];

    if(!side) return NULL;

    switch(special)
    {
    default: return NULL; // Not a scroller.

#if __JDOOM__ || __JDOOM64__ || __JHERETIC__
    case 48:  ///< Tagless, scroll left.
# if __JDOOM64__
    case 150: ///< Tagless, scroll right.
# elif __JDOOM__
    case 85:  ///< Tagless, scroll right (BOOM).
# elif __JHERETIC__
    case 99:  ///< Tagless, scroll right.
# endif
        offset[0] = (special == 48? 1 : -1);
        offset[1] = 0;
        break;
#endif //  __JDOOM__ || __JDOOM64__ || __JHERETIC__

#if __JDOOM64__
    case 2561: ///< Tagless, scroll up.
    case 2562: ///< Tagless, scroll down.
        offset[0] = 0;
        offset[1] = (special == 2561? 1 : -1);
        break;

    case 2080: ///< Tagless, scroll up/left.
    case 2614: ///< Tagless, scroll up/right.
        offset[0] = (special == 2614? 1 : -1);
        offset[1] =  1;
        break;
#endif // __JDOOM64__

#if __JHEXEN__
    case 100:  ///< Tagless, scroll left at speed.
    case 101: {  ///< Tagless, scroll right at speed.
        xline_t* xline = P_ToXLine(P_GetPtrp(side, DMU_LINEDEF));
        float speed = FIX2FLT(xline->arg1 << 10);
        offset[0] = (special == 100? speed : -speed);
        offset[1] = 0;
        break; }

    case 102: ///< Tagless, scroll up at speed.
    case 103: {  ///< Tagless, scroll down speed.
        xline_t* xline = P_ToXLine(P_GetPtrp(side, DMU_LINEDEF));
        float speed = FIX2FLT(xline->arg1 << 10);
        offset[0] = 0;
        offset[1] = (special == 102? speed : -speed);
        break; }
#endif // __JHEXEN__

#if __JDOOM__
    case 255: ///< Tagless, scroll by material origin (BOOM).
        P_GetFloatpv(side, DMU_MIDDLE_MATERIAL_OFFSET_XY, offset);
        offset[0] = -offset[0];
        break;
#endif
    }

    elementBits  = (1 << SS_MIDDLE) | (1 << SS_BOTTOM) | (1 << SS_TOP);
    return spawnMaterialOriginScroller(side, elementBits, offset);
}

scroll_t* P_SpawnSectorMaterialOriginScroller(Sector* sector, uint planeId, short special)
{
#define SCROLLUNIT (8.f/35*2)

    int elementBits;
    float offset[2];

    // Don't spawn a scroller with an invalid surface reference.
    if(!sector || !(planeId == PLN_FLOOR || planeId == PLN_CEILING)) return NULL;

    /// @todo $nplanes

    switch(special)
    {
    default: return NULL; // Not a scroller.

#if __JHERETIC__ || __JHEXEN__
# if __JHERETIC__
    case 25: ///< Scroll north.
    case 26:
    case 27:
    case 28:
    case 29:
# else // __JHEXEN__
    case 201: ///< Scroll north.
    case 202:
    case 203:
# endif
# if __JHERETIC__
        // A bug in the original game prevented all but eastward scrollers from working.
        if(!cfg.fixPlaneScrollMaterialsEastOnly) break;
# endif

        offset[0] = 0;
# if __JHERETIC__
        offset[1] = -(SCROLLUNIT * (1 + (special - 25)*2));
# else // __JHEXEN__
        offset[1] = -(SCROLLUNIT * (1 + special - 201));
# endif
        break;

# if __JHERETIC__
    case 20: ///< Scroll east.
    case 21:
    case 22:
    case 23:
    case 24:
# else // __JHEXEN__
    case 204: ///< Scroll east.
    case 205:
    case 206:
# endif
# if __JHERETIC__
        offset[0] = -(SCROLLUNIT * (1 + (special - 20)*2));
# else // __JHEXEN__
        offset[0] = -(SCROLLUNIT * (1 + special - 204));
# endif
        offset[1] = 0;
        break;
# if __JHERETIC__
    case 30: ///< Scroll south.
    case 31:
    case 32:
    case 33:
    case 34:
# else // __JHEXEN__
    case 207: ///< Scroll south.
    case 208:
    case 209:
# endif
# if __JHERETIC__
        // A bug in the original game prevented all but eastward scrollers from working.
        if(!cfg.fixPlaneScrollMaterialsEastOnly) break;
#endif

        offset[0] = 0;
# if __JHERETIC__
        offset[1] = SCROLLUNIT * (1 + (special - 30)*2);
# else // __JHEXEN__
        offset[1] = SCROLLUNIT * (1 + special - 207);
# endif
        break;

# if __JHERETIC__
    case 35: ///< Scroll west.
    case 36:
    case 37:
    case 38:
    case 39:
# else // __JHEXEN__
    case 210: ///< Scroll west.
    case 211:
    case 212:
# endif
# if __JHERETIC__
        // A bug in the original game prevented all but eastward scrollers from working.
        if(!cfg.fixPlaneScrollMaterialsEastOnly) break;
# endif

# if __JHERETIC__
        offset[0] = SCROLLUNIT * (1 + (special - 35)*2);
# else // __JHEXEN__
        offset[VX] = SCROLLUNIT * (1 + special - 210);
# endif
        offset[1] = 0;
        break;
#endif // __JHERETIC__ || __JHEXEN__

#if __JHERETIC__
    case 4: ///< Scroll east (lava damage).
        offset[0] = -(SCROLLUNIT * 8 * (1 + special - 4));
        offset[1] = 0;
        break;
#endif

#if __JHEXEN__
    case 213: ///< Scroll northwest.
    case 214:
    case 215:
        offset[0] = SCROLLUNIT * (1 + special - 213);
        offset[1] = -(SCROLLUNIT * (1 + special - 213));
        break;

    case 216: ///< Scroll northeast.
    case 217:
    case 218:
        offset[0] = -(SCROLLUNIT * (1 + special - 216));
        offset[1] = -(SCROLLUNIT * (1 + special - 216));
        break;

    case 219: ///< Scroll southeast.
    case 220:
    case 221:
        offset[0] = -(SCROLLUNIT * (1 + special - 219));
        offset[1] = SCROLLUNIT * (1 + special - 219);
        break;

    case 222: ///< Scroll southwest.
    case 223:
    case 224:
        offset[0] = SCROLLUNIT * (1 + special - 222);
        offset[1] = SCROLLUNIT * (1 + special - 222);
        break;
#endif
    }

    elementBits = 1 << planeId;
    return spawnMaterialOriginScroller(sector, elementBits, offset);

#undef SCROLLUNIT
}
