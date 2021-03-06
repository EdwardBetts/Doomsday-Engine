/** @file common.c  Top-level libcommon routines.
 *
 * @authors Copyright © 2012-2017 Jaakko Keränen <jaakko.keranen@iki.fi>
 * @authors Copyright © 2013-2014 Daniel Swanson <danij@dengine.net>
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
#include "g_common.h"

int Common_GetInteger(int id)
{
    switch(id)
    {
    case DD_GAME_RECOMMENDS_SAVING:
        // The engine will use this as a hint whether to remind the user to
        // manually save the game before, e.g., upgrading to a new version.
        return G_GameState() == GS_MAP;

    default: break;
    }

    return 0;
}

#ifdef __JDOOM__
void fastMonstersChanged()
{
    G_Ruleset_UpdateDefaults();
}
#endif

void Common_Register()
{
    // Movement
    C_VAR_FLOAT("player-move-speed",    &cfg.common.playerMoveSpeed,    0, 0, 1);
    C_VAR_INT  ("player-jump",          &cfg.common.jumpEnabled,        0, 0, 1);
    C_VAR_FLOAT("player-jump-power",    &cfg.common.jumpPower,          0, 0, 100);
    C_VAR_BYTE ("player-air-movement",  &cfg.common.airborneMovement,   0, 0, 32);

    // Gameplay
    C_VAR_BYTE ("sound-switch-origin",  &cfg.common.switchSoundOrigin,  0, 0, 1);
#ifdef __JDOOM__
    C_VAR_BYTE2("game-monsters-fast",   &cfg.common.defaultRuleFastMonsters, 0, 0, 1, fastMonstersChanged);
#endif
}
