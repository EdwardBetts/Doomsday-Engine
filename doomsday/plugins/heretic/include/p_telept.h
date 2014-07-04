/** @file p_telept.h  Intra map teleporters.
 *
 * @authors Copyright © 2009-2014 Daniel Swanson <danij@dengine.net>
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

#ifndef LIBHERETIC_PLAY_TELEPT_H
#define LIBHERETIC_PLAY_TELEPT_H

#ifndef __JHERETIC__
#  error "Using jHeretic headers without __JHERETIC__"
#endif

#include "doomsday.h"
#include "p_mobj.h"

#define TELEFOGHEIGHTF      (32)

#ifdef __cplusplus
extern "C" {
#endif

dd_bool EV_Teleport(Line *line, int side, mobj_t *thing, dd_bool spawnFog);
void P_ArtiTele(player_t *player);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LIBHERETIC_PLAY_TELEPT_H
