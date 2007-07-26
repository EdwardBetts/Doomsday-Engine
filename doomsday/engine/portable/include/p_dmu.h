/**\file
 *\section License
 * License: GPL
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright © 2006-2007 Jaakko Keränen <skyjake@dengine.net>
 *\author Copyright © 2006 Daniel Swanson <danij@dengine.net>
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
 * p_dmu.h: Map Update API
 *
 * Engine-internal header for DMU.
 */

#ifndef __DOOMSDAY_MAP_UPDATE_H__
#define __DOOMSDAY_MAP_UPDATE_H__

void        P_InitMapUpdate(void);
void       *P_AllocDummy(int type, void* extraData);
void        P_FreeDummy(void* dummy);
int         P_DummyType(void* dummy);
boolean     P_IsDummy(void* dummy);
void       *P_DummyExtraData(void* dummy);

uint        P_ToIndex(const void* ptr);

const char *DMU_Str(uint prop);

#ifndef NDEBUG
# define ASSERT_DMU_TYPE(ptr, dmuType) \
    if(!ptr || ((runtime_mapdata_header_t*)ptr)->type != dmuType) \
        Con_Error("ASSERT_DMU_TYPE failure on line %i in "__FILE__". " #ptr " is not %s.\n", __LINE__, DMU_Str(dmuType));
#else
# define ASSERT_DMU_TYPE(ptr, dmuType)
#endif

#endif
