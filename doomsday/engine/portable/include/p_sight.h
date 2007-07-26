/**\file
 *\section License
 * License: GPL
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright © 2003-2007 Jaakko Keränen <skyjake@dengine.net>
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
 * de_play.h: Line of Sight Testing
 */

#ifndef __DOOMSDAY_PLAY_SIGHT_H__
#define __DOOMSDAY_PLAY_SIGHT_H__

extern fixed_t  topslope;
extern fixed_t  bottomslope;

boolean         P_CheckReject(sector_t *sec1, sector_t *sec2);
boolean         P_CheckFrustum(int plrNum, mobj_t *mo);
boolean         P_CheckSight(mobj_t *t1, mobj_t *t2);
boolean         P_CheckLineSight(float from[3], float to[3]);

#endif
