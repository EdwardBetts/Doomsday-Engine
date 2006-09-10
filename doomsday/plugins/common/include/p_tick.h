/**\file
 *\section Copyright and License Summary
 * License: GPL
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright � 2006 Jaakko Ker�nen <skyjake@dengine.net>
 *\author Copyright � 2006 Daniel Swanson <danij@dengine.net>
 *
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
 * p_tick.c: Top-level tick stuff.
 */

#ifndef __COMMON_TICK_H__
#define __COMMON_TICK_H__

void    P_RunPlayers(void);
boolean P_IsPaused(void);
void    P_DoTick(void);

#endif
