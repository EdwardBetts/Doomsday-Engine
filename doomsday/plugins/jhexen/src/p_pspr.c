/**\file
 *\section License
 * License: GPL
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright © 2003-2009 Jaakko Keränen <jaakko.keranen@iki.fi>
 *\author Copyright © 2006-2009 Daniel Swanson <danij@dengine.net>
 *\author Copyright © 1999 Activision
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

/**
 * p_pspr.c: Weapon sprite animation.
 *
 * Weapon sprite animation, weapon objects.
 * Action functions for weapons.
 */

// HEADER FILES ------------------------------------------------------------

#include <math.h>

#include "jhexen.h"

#include "p_player.h"
#include "p_map.h"
#include "p_tick.h"

// MACROS ------------------------------------------------------------------

#define LOWERSPEED          (6)
#define RAISESPEED          (6)
#define WEAPONBOTTOM        (128)
#define WEAPONTOP           (32)

#define ZAGSPEED            (1)
#define MAX_ANGLE_ADJUST    (5*ANGLE_1)
#define HAMMER_RANGE        (MELEERANGE+MELEERANGE/2)
#define AXERANGE            (2.25*MELEERANGE)
#define FLAMESPEED          (0.45)
#define FLAMEROTSPEED       (2)

#define SHARDSPAWN_LEFT     (1)
#define SHARDSPAWN_RIGHT    (2)
#define SHARDSPAWN_UP       (4)
#define SHARDSPAWN_DOWN     (8)

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

float bulletSlope;

weaponinfo_t weaponInfo[NUM_WEAPON_TYPES][NUM_PLAYER_CLASSES] = {
    {                           // First Weapons
     {                          // Fighter First Weapon - Punch
     {
      GM_ANY,                   // Gamemode bits
      {0, 0},                   // type: mana1 | mana2
      {0, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_PUNCHUP,                // upstate
      0,                        // raise sound id
      S_PUNCHDOWN,              // downstate
      S_PUNCHREADY,             // readystate
      0,                        // readysound
      S_PUNCHATK1_1,            // atkstate
      S_PUNCHATK1_1,            // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Cleric First Weapon - Mace
      GM_ANY,                   // Gamemode bits
      {0, 0},                   // type: mana1 | mana2
      {0, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_CMACEUP,                // upstate
      0,                        // raise sound id
      S_CMACEDOWN,              // downstate
      S_CMACEREADY,             // readystate
      0,                        // readysound
      S_CMACEATK_1,             // atkstate
      S_CMACEATK_1,             // holdatkstate
      S_NULL                    // flashstate
      }
      },
     {
     {                          // Mage First Weapon - Wand
      GM_ANY,                   // Gamemode bits
      {0, 0},                   // type: mana1 | mana2
      {0, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_MWANDUP,
      0,                        // raise sound id
      S_MWANDDOWN,
      S_MWANDREADY,
      0,                        // readysound
      S_MWANDATK_1,
      S_MWANDATK_1,
      S_NULL
      }
     },
     {
     {                          // Pig - Snout
      GM_ANY,                   // Gamemode bits
      {0, 0},                   // type: mana1 | mana2
      {0, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_SNOUTUP,                // upstate
      0,                        // raise sound id
      S_SNOUTDOWN,              // downstate
      S_SNOUTREADY,             // readystate
      0,                        // readysound
      S_SNOUTATK1,              // atkstate
      S_SNOUTATK1,              // holdatkstate
      S_NULL                    // flashstate
      }
      }
     },
    {                           // Second Weapons
    {
     {                          // Fighter - Axe
      GM_ANY,                   // Gamemode bits
      {1, 0},                   // type: mana1 | mana2
      {2, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_FAXEUP,                 // upstate
      0,                        // raise sound id
      S_FAXEDOWN,               // downstate
      S_FAXEREADY,              // readystate
      0,                        // readysound
      S_FAXEATK_1,              // atkstate
      S_FAXEATK_1,              // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Cleric - Serpent Staff
      GM_ANY,                   // Gamemode bits
      {1, 0},                   // type: mana1 | mana2
      {1, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_CSTAFFUP,               // upstate
      0,                        // raise sound id
      S_CSTAFFDOWN,             // downstate
      S_CSTAFFREADY,            // readystate
      0,                        // readysound
      S_CSTAFFATK_1,            // atkstate
      S_CSTAFFATK_1,            // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Mage - Cone of shards
      GM_ANY,                   // Gamemode bits
      {1, 0},                   // type: mana1 | mana2
      {3, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_CONEUP,                 // upstate
      0,                        // raise sound id
      S_CONEDOWN,               // downstate
      S_CONEREADY,              // readystate
      0,                        // readysound
      S_CONEATK1_1,             // atkstate
      S_CONEATK1_3,             // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Pig - Snout
      GM_ANY,                   // Gamemode bits
      {0, 0},                   // type: mana1 | mana2
      {0, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_SNOUTUP,                // upstate
      0,                        // raise sound id
      S_SNOUTDOWN,              // downstate
      S_SNOUTREADY,             // readystate
      0,                        // readysound
      S_SNOUTATK1,              // atkstate
      S_SNOUTATK1,              // holdatkstate
      S_NULL                    // flashstate
      }
     }
    },
    {                           // Third Weapons
    {
     {                          // Fighter - Hammer
      GM_ANY,                   // Gamemode bits
      {0, 1},                   // type: mana1 | mana2
      {0, 3},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_FHAMMERUP,              // upstate
      0,                        // raise sound id
      S_FHAMMERDOWN,            // downstate
      S_FHAMMERREADY,           // readystate
      0,                        // readysound
      S_FHAMMERATK_1,           // atkstate
      S_FHAMMERATK_1,           // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Cleric - Flame Strike
      GM_ANY,                   // Gamemode bits
      {0, 1},                   // type: mana1 | mana2
      {0, 4},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_CFLAMEUP,               // upstate
      0,                        // raise sound id
      S_CFLAMEDOWN,             // downstate
      S_CFLAMEREADY1,           // readystate
      0,                        // readysound
      S_CFLAMEATK_1,            // atkstate
      S_CFLAMEATK_1,            // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Mage - Lightning
      GM_ANY,                   // Gamemode bits
      {0, 1},                   // type: mana1 | mana2
      {0, 5},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_MLIGHTNINGUP,           // upstate
      0,                        // raise sound id
      S_MLIGHTNINGDOWN,         // downstate
      S_MLIGHTNINGREADY,        // readystate
      0,                        // readysound
      S_MLIGHTNINGATK_1,        // atkstate
      S_MLIGHTNINGATK_1,        // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Pig - Snout
      GM_ANY,                   // Gamemode bits
      {0, 0},                   // type: mana1 | mana2
      {0, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_SNOUTUP,                // upstate
      0,                        // raise sound id
      S_SNOUTDOWN,              // downstate
      S_SNOUTREADY,             // readystate
      0,                        // readysound
      S_SNOUTATK1,              // atkstate
      S_SNOUTATK1,              // holdatkstate
      S_NULL                    // flashstate
      }
     }
    },
    {                           // Fourth Weapons
     {
     {                          // Fighter - Rune Sword
      GM_ANY,                   // Gamemode bits
      {1, 1},                   // type: mana1 | mana2
      {14, 14},                 // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_FSWORDUP,               // upstate
      0,                        // raise sound id
      S_FSWORDDOWN,             // downstate
      S_FSWORDREADY,            // readystate
      0,                        // readysound
      S_FSWORDATK_1,            // atkstate
      S_FSWORDATK_1,            // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Cleric - Holy Symbol
      GM_ANY,                   // Gamemode bits
      {1, 1},                   // type: mana1 | mana2
      {18, 18},                 // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_CHOLYUP,                // upstate
      0,                        // raise sound id
      S_CHOLYDOWN,              // downstate
      S_CHOLYREADY,             // readystate
      0,                        // readysound
      S_CHOLYATK_1,             // atkstate
      S_CHOLYATK_1,             // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Mage - Staff
      GM_ANY,                   // Gamemode bits
      {1, 1},                   // type: mana1 | mana2
      {15, 15},                 // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_MSTAFFUP,               // upstate
      0,                        // raise sound id
      S_MSTAFFDOWN,             // downstate
      S_MSTAFFREADY,            // readystate
      0,                        // readysound
      S_MSTAFFATK_1,            // atkstate
      S_MSTAFFATK_1,            // holdatkstate
      S_NULL                    // flashstate
      }
     },
     {
     {                          // Pig - Snout
      GM_ANY,                   // Gamemode bits
      {0, 0},                   // type: mana1 | mana2
      {0, 0},                   // pershot: mana1 | mana2
      true,                     // autofire when raised if fire held
      S_SNOUTUP,                // upstate
      0,                        // raise sound id
      S_SNOUTDOWN,              // downstate
      S_SNOUTREADY,             // readystate
      0,                        // readysound
      S_SNOUTATK1,              // atkstate
      S_SNOUTATK1,              // holdatkstate
      S_NULL                    // flashstate
      }
     }
    }
};

// PRIVATE DATA DEFINITIONS ------------------------------------------------

// CODE --------------------------------------------------------------------

void R_GetWeaponBob(int player, float* x, float* y)
{
    if(x)
    {
        if(players[player].morphTics > 0)
            *x = 0;
        else
            *x = 1 + (PLRPROFILE.psprite.bob * players[player].bob) *
                FIX2FLT(finecosine[(128 * mapTime) & FINEMASK]);
    }

    if(y)
    {
        if(players[player].morphTics > 0)
            *y = 0;
        else
            *y = 32 + (PLRPROFILE.psprite.bob * players[player].bob) *
                FIX2FLT(finesine[(128 * mapTime) & FINEMASK & (FINEANGLES / 2 - 1)]);
    }
}

/**
 * Offset in state->misc1/2.
 */
void P_SetPSpriteOffset(pspdef_t *psp, player_t *plr, state_t *state)
{
    ddpsprite_t *ddpsp = plr->plr->pSprites;

    // Clear the Offset flag by default.
    //ddpsp->flags &= ~DDPSPF_OFFSET;

    if(state->misc[0])
    {
        // Set coordinates.
        psp->pos[VX] = (float) state->misc[0];
        //ddpsp->flags |= DDPSPF_OFFSET;
        ddpsp->offset[VX] = (float) state->misc[0];
    }

    if(state->misc[1])
    {
        psp->pos[VY] = (float) state->misc[1];
        //ddpsp->flags |= DDPSPF_OFFSET;
        ddpsp->offset[VY] = (float) state->misc[1];
    }
}

void P_SetPsprite(player_t *plr, int position, statenum_t stnum)
{
    pspdef_t       *psp;
    state_t        *state;

    psp = &plr->pSprites[position];
    do
    {
        if(!stnum)
        {   // Object removed itself.
            psp->state = NULL;
            break;
        }

        state = &STATES[stnum];
        psp->state = state;
        psp->tics = state->tics; // could be 0
        P_SetPSpriteOffset(psp, plr, state);

        if(state->action)
        {   // Call action routine.
            state->action(plr, psp);
            if(!psp->state)
            {
                break;
            }
        }

        stnum = psp->state->nextState;
    } while(!psp->tics); // An initial state of 0 could cycle through.
}

/**
 * Identical to P_SetPsprite, without calling the action function.
 */
void P_SetPspriteNF(player_t *plr, int position, statenum_t stnum)
{
    pspdef_t       *psp;
    state_t        *state;

    psp = &plr->pSprites[position];
    do
    {
        if(!stnum)
        {   // Object removed itself.
            psp->state = NULL;
            break;
        }

        state = &STATES[stnum];
        psp->state = state;
        psp->tics = state->tics; // could be 0

        P_SetPSpriteOffset(psp, plr, state);
        stnum = psp->state->nextState;
    } while(!psp->tics); // An initial state of 0 could cycle through.
}

void P_ActivateMorphWeapon(player_t *plr)
{
    plr->pendingWeapon = WT_NOCHANGE;
    plr->pSprites[PS_WEAPON].pos[VY] = WEAPONTOP;
    plr->readyWeapon = WT_FIRST; // Snout is the first weapon
    plr->update |= PSF_WEAPONS;
    P_SetPsprite(plr, PS_WEAPON, S_SNOUTREADY);
}

void P_PostMorphWeapon(player_t *plr, weapontype_t weapon)
{
    plr->pendingWeapon = WT_NOCHANGE;
    plr->readyWeapon = weapon;
    plr->pSprites[PS_WEAPON].pos[VY] = WEAPONBOTTOM;
    plr->update |= PSF_WEAPONS;
    P_SetPsprite(plr, PS_WEAPON, weaponInfo[weapon][plr->pClass].mode[0].upState);
}

/**
 * Starts bringing the pending weapon up from the bottom of the screen.
 */
void P_BringUpWeapon(player_t *plr)
{
    statenum_t newState;
    weaponmodeinfo_t *wminfo;

    wminfo = WEAPON_INFO(plr->pendingWeapon, plr->pClass, 0);

    newState = wminfo->upState;
    if(plr->pClass == PCLASS_FIGHTER && plr->pendingWeapon == WT_SECOND &&
       plr->ammo[AT_BLUEMANA].owned > 0)
    {
        newState = S_FAXEUP_G;
    }

    if(plr->pendingWeapon == WT_NOCHANGE)
        plr->pendingWeapon = plr->readyWeapon;

    if(wminfo->raiseSound)
        S_StartSound(wminfo->raiseSound, plr->plr->mo);

    plr->pendingWeapon = WT_NOCHANGE;
    plr->pSprites[PS_WEAPON].pos[VY] = WEAPONBOTTOM;
    P_SetPsprite(plr, PS_WEAPON, newState);
}

void P_FireWeapon(player_t *plr)
{
    statenum_t attackState;

    if(!P_CheckAmmo(plr))
        return;

    // Psprite state.
    P_MobjChangeState(plr->plr->mo, PCLASS_INFO(plr->pClass)->attackState);
    if(plr->pClass == PCLASS_FIGHTER && plr->readyWeapon == WT_SECOND &&
       plr->ammo[AT_BLUEMANA].owned > 0)
    {   // Glowing axe.
        attackState = S_FAXEATK_G1;
    }
    else
    {
        if(plr->refire)
            attackState =
                weaponInfo[plr->readyWeapon][plr->pClass].mode[0].holdAttackState;
        else
            attackState =
                weaponInfo[plr->readyWeapon][plr->pClass].mode[0].attackState;
    }

    P_SetPsprite(plr, PS_WEAPON, attackState);
    P_NoiseAlert(plr->plr->mo, plr->plr->mo);

    plr->update |= PSF_AMMO;

    // Psprite state.
    plr->plr->pSprites[0].state = DDPSP_FIRE;
}

/**
 * The player died, so put the weapon away.
 */
void P_DropWeapon(player_t *plr)
{
    P_SetPsprite(plr, PS_WEAPON,
                 weaponInfo[plr->readyWeapon][plr->pClass].mode[0].downState);
}

/**
 * The player can fire the weapon or change to another weapon at this time.
 */
void C_DECL A_WeaponReady(player_t *plr, pspdef_t *psp)
{
    weaponmodeinfo_t *wminfo;
    ddpsprite_t *ddpsp;

    // Change plr from attack state
    if(plr->plr->mo->state >= &STATES[PCLASS_INFO(plr->pClass)->attackState] &&
       plr->plr->mo->state <= &STATES[PCLASS_INFO(plr->pClass)->attackEndState])
    {
        P_MobjChangeState(plr->plr->mo, PCLASS_INFO(plr->pClass)->normalState);
    }

    if(plr->readyWeapon != WT_NOCHANGE)
    {
        wminfo = WEAPON_INFO(plr->readyWeapon, plr->pClass, 0);

        // A weaponready sound?
        if(psp->state == &STATES[wminfo->readyState] && wminfo->readySound)
            S_StartSound(wminfo->readySound, plr->plr->mo);

        // Check for change, if plr is dead, put the weapon away.
        if(plr->pendingWeapon != WT_NOCHANGE || !plr->health)
        {   //  (pending weapon should allready be validated)
            P_SetPsprite(plr, PS_WEAPON, wminfo->downState);
            return;
        }
    }

    // Check for autofire.
    if(plr->brain.attack)
    {
        wminfo = WEAPON_INFO(plr->readyWeapon, plr->pClass, 0);

        if(!plr->attackDown || wminfo->autoFire)
        {
            plr->attackDown = true;
            P_FireWeapon(plr);
            return;
        }
    }
    else
    {
        plr->attackDown = false;
    }

    ddpsp = plr->plr->pSprites;

    if(!plr->morphTics)
    {
        // Bob the weapon based on movement speed.
        R_GetWeaponBob(plr - players, &psp->pos[0], &psp->pos[1]);

        ddpsp->offset[0] = ddpsp->offset[1] = 0;
    }

    // Psprite state.
    ddpsp->state = DDPSP_BOBBING;
}

/**
 * The player can re fire the weapon without lowering it entirely.
 */
void C_DECL A_ReFire(player_t* plr, pspdef_t* psp)
{
    if((plr->brain.attack) &&
       plr->pendingWeapon == WT_NOCHANGE && plr->health)
    {
        plr->refire++;
        P_FireWeapon(plr);
    }
    else
    {
        plr->refire = 0;
        P_CheckAmmo(plr);
    }
}

void C_DECL A_Lower(player_t *plr, pspdef_t *psp)
{
    // Psprite state.
    plr->plr->pSprites[0].state = DDPSP_DOWN;

    if(plr->morphTics)
    {
        psp->pos[VY] = WEAPONBOTTOM;
    }
    else
    {
        psp->pos[VY] += LOWERSPEED;
    }

    if(psp->pos[VY] < WEAPONBOTTOM)
    {   // Not lowered all the way yet.
        return;
    }

    if(plr->pState == PST_DEAD)
    {   // Player is dead, so don't bring up a pending weapon.
        psp->pos[VY] = WEAPONBOTTOM;
        return;
    }

    if(!plr->health)
    {   // Player is dead, so keep the weapon off screen.
        P_SetPsprite(plr, PS_WEAPON, S_NULL);
        return;
    }

    plr->readyWeapon = plr->pendingWeapon;
    plr->update |= PSF_WEAPONS;
    P_BringUpWeapon(plr);
}

void C_DECL A_Raise(player_t *plr, pspdef_t *psp)
{
    // Psprite state.
    plr->plr->pSprites[0].state = DDPSP_UP;

    psp->pos[VY] -= RAISESPEED;
    if(psp->pos[VY] > WEAPONTOP)
    {   // Not raised all the way yet.
        return;
    }

    psp->pos[VY] = WEAPONTOP;
    if(plr->pClass == PCLASS_FIGHTER && plr->readyWeapon == WT_SECOND &&
       plr->ammo[AT_BLUEMANA].owned > 0)
    {
        P_SetPsprite(plr, PS_WEAPON, S_FAXEREADY_G);
    }
    else
    {
        P_SetPsprite(plr, PS_WEAPON,
                     weaponInfo[plr->readyWeapon][plr->pClass].mode[0].
                     readyState);
    }
}

void AdjustPlayerAngle(mobj_t *pmo)
{
    angle_t angle;
    int     difference;

    angle = R_PointToAngle2(pmo->pos[VX], pmo->pos[VY],
                            lineTarget->pos[VX], lineTarget->pos[VY]);
    difference = (int) angle - (int) pmo->angle;
    if(abs(difference) > MAX_ANGLE_ADJUST)
    {
        pmo->angle += difference > 0 ? MAX_ANGLE_ADJUST : -MAX_ANGLE_ADJUST;
    }
    else
    {
        pmo->angle = angle;
    }
    pmo->player->plr->flags |= DDPF_FIXANGLES;
}

void C_DECL A_SnoutAttack(player_t *plr, pspdef_t *psp)
{
    angle_t     angle;
    int         damage;
    float       slope;

    damage = 3 + (P_Random() & 3);
    angle = plr->plr->mo->angle;
    slope = P_AimLineAttack(plr->plr->mo, angle, MELEERANGE);

    puffType = MT_SNOUTPUFF;
    puffSpawned = NULL;

    P_LineAttack(plr->plr->mo, angle, MELEERANGE, slope, damage);
    S_StartSound(SFX_PIG_ACTIVE1 + (P_Random() & 1), plr->plr->mo);

    if(lineTarget)
    {
        AdjustPlayerAngle(plr->plr->mo);

        if(puffSpawned)
        {   // Bit something.
            S_StartSound(SFX_PIG_ATTACK, plr->plr->mo);
        }
    }
}

void C_DECL A_FHammerAttack(player_t *plr, pspdef_t *psp)
{
    int         i;
    angle_t     angle;
    mobj_t     *mo = plr->plr->mo;
    int         damage;
    float       power;
    float       slope;

    damage = 60 + (P_Random() & 63);
    power = 10;
    puffType = MT_HAMMERPUFF;
    for(i = 0; i < 16; ++i)
    {
        angle = mo->angle + i * (ANG45 / 32);
        slope = P_AimLineAttack(mo, angle, HAMMER_RANGE);
        if(lineTarget)
        {
            P_LineAttack(mo, angle, HAMMER_RANGE, slope, damage);
            AdjustPlayerAngle(mo);
            if((lineTarget->flags & MF_COUNTKILL) || lineTarget->player)
            {
                P_ThrustMobj(lineTarget, angle, power);
            }

            mo->special1 = false; // Don't throw a hammer.
            goto hammerdone;
        }

        angle = mo->angle - i * (ANG45 / 32);
        slope = P_AimLineAttack(mo, angle, HAMMER_RANGE);
        if(lineTarget)
        {
            P_LineAttack(mo, angle, HAMMER_RANGE, slope, damage);
            AdjustPlayerAngle(mo);
            if((lineTarget->flags & MF_COUNTKILL) || lineTarget->player)
            {
                P_ThrustMobj(lineTarget, angle, power);
            }

            mo->special1 = false; // Don't throw a hammer.
            goto hammerdone;
        }
    }

    // Didn't find any targets in meleerange, so set to throw out a hammer.
    puffSpawned = NULL;

    angle = mo->angle;
    slope = P_AimLineAttack(mo, angle, HAMMER_RANGE);
    P_LineAttack(mo, angle, HAMMER_RANGE, slope, damage);
    if(puffSpawned)
    {
        mo->special1 = false;
    }
    else
    {
        mo->special1 = true;
    }

  hammerdone:
    if(plr->ammo[AT_GREENMANA].owned <
       weaponInfo[plr->readyWeapon][plr->pClass].mode[0].perShot[AT_GREENMANA])
    {   // Don't spawn a hammer if the plr doesn't have enough mana.
        mo->special1 = false;
    }
}

void C_DECL A_FHammerThrow(player_t *plr, pspdef_t *psp)
{
    mobj_t             *pmo;

    if(!plr->plr->mo->special1)
        return;

    P_ShotAmmo(plr);

    pmo = P_SpawnPlayerMissile(MT_HAMMER_MISSILE, plr->plr->mo);
    if(pmo)
        pmo->special1 = 0;
}

void C_DECL A_FSwordAttack(player_t *plr, pspdef_t *psp)
{
    mobj_t             *mo;

    P_ShotAmmo(plr);

    mo = plr->plr->mo;
    P_SPMAngleXYZ(MT_FSWORD_MISSILE, mo->pos[VX], mo->pos[VY], mo->pos[VZ] - 10,
                  mo, mo->angle + ANG45 / 4);
    P_SPMAngleXYZ(MT_FSWORD_MISSILE, mo->pos[VX], mo->pos[VY], mo->pos[VZ] - 5,
                  mo, mo->angle + ANG45 / 8);
    P_SPMAngleXYZ(MT_FSWORD_MISSILE, mo->pos[VX], mo->pos[VY], mo->pos[VZ],
                  mo, mo->angle);
    P_SPMAngleXYZ(MT_FSWORD_MISSILE, mo->pos[VX], mo->pos[VY], mo->pos[VZ] + 5,
                  mo, mo->angle - ANG45 / 8);
    P_SPMAngleXYZ(MT_FSWORD_MISSILE, mo->pos[VX], mo->pos[VY], mo->pos[VZ] + 10,
                  mo, mo->angle - ANG45 / 4);
    S_StartSound(SFX_FIGHTER_SWORD_FIRE, mo);
}

void C_DECL A_FSwordAttack2(mobj_t *mo)
{
    angle_t         angle = mo->angle;

    P_SpawnMissileAngle(MT_FSWORD_MISSILE, mo, angle + ANG45 / 4, 0);
    P_SpawnMissileAngle(MT_FSWORD_MISSILE, mo, angle + ANG45 / 8, 0);
    P_SpawnMissileAngle(MT_FSWORD_MISSILE, mo, angle, 0);
    P_SpawnMissileAngle(MT_FSWORD_MISSILE, mo, angle - ANG45 / 8, 0);
    P_SpawnMissileAngle(MT_FSWORD_MISSILE, mo, angle - ANG45 / 4, 0);
    S_StartSound(SFX_FIGHTER_SWORD_FIRE, mo);
}

void C_DECL A_FSwordFlames(mobj_t *mo)
{
    int             i;
    angle_t         angle;
    float           pos[3];

    for(i = 1 + (P_Random() & 3); i; i--)
    {
        pos[VX] = mo->pos[VX] + FIX2FLT((P_Random() - 128) << 12);
        pos[VY] = mo->pos[VY] + FIX2FLT((P_Random() - 128) << 12);
        pos[VX] = mo->pos[VZ] + FIX2FLT((P_Random() - 128) << 11);
        angle = R_PointToAngle2(mo->pos[VX], mo->pos[VY], pos[VX], pos[VY]);

        P_SpawnMobj3fv(MT_FSWORD_FLAME, pos, angle);
    }
}

void C_DECL A_MWandAttack(player_t *plr, pspdef_t *psp)
{
    P_SpawnPlayerMissile(MT_MWAND_MISSILE, plr->plr->mo);
    S_StartSound(SFX_MAGE_WAND_FIRE, plr->plr->mo);
}

void C_DECL A_LightningReady(player_t *plr, pspdef_t *psp)
{
    A_WeaponReady(plr, psp);
    if(P_Random() < 160)
    {
        S_StartSound(SFX_MAGE_LIGHTNING_READY, plr->plr->mo);
    }
}

void C_DECL A_LightningClip(mobj_t *mo)
{
    mobj_t         *cMo, *target = 0;
    int             zigZag;

    if(mo->type == MT_LIGHTNING_FLOOR)
    {
        mo->pos[VZ] = mo->floorZ;
        target = mo->lastEnemy->tracer;
    }
    else if(mo->type == MT_LIGHTNING_CEILING)
    {
        mo->pos[VZ] = mo->ceilingZ - mo->height;
        target = mo->tracer;
    }

    if(mo->type == MT_LIGHTNING_FLOOR)
    {   // Floor lightning zig-zags, and forces the ceiling lightning to mimic.
        cMo = mo->lastEnemy;
        zigZag = P_Random();
        if((zigZag > 128 && mo->special1 < 2) || mo->special1 < -2)
        {
            P_ThrustMobj(mo, mo->angle + ANG90, ZAGSPEED);
            if(cMo)
            {
                P_ThrustMobj(cMo, mo->angle + ANG90, ZAGSPEED);
            }
            mo->special1++;
        }
        else
        {
            P_ThrustMobj(mo, mo->angle - ANG90, ZAGSPEED);
            if(cMo)
            {
                P_ThrustMobj(cMo, cMo->angle - ANG90, ZAGSPEED);
            }
            mo->special1--;
        }
    }

    if(target)
    {
        if(target->health <= 0)
        {
            P_ExplodeMissile(mo);
        }
        else
        {
            mo->angle = R_PointToAngle2(mo->pos[VX], mo->pos[VY],
                                        target->pos[VX], target->pos[VY]);
            mo->mom[MX] = mo->mom[MY] = 0;
            P_ThrustMobj(mo, mo->angle, mo->info->speed / 2);
        }
    }
}

void C_DECL A_LightningZap(mobj_t *mo)
{
    mobj_t         *pmo;
    float           deltaZ;

    A_LightningClip(mo);

    mo->health -= 8;
    if(mo->health <= 0)
    {
        P_MobjChangeState(mo, P_GetState(mo->type, SN_DEATH));
        return;
    }

    if(mo->type == MT_LIGHTNING_FLOOR)
    {
        deltaZ = 10;
    }
    else
    {
        deltaZ = -10;
    }

    pmo = P_SpawnMobj3f(MT_LIGHTNING_ZAP,
                        mo->pos[VX] + (FIX2FLT(P_Random() - 128) * mo->radius / 256),
                        mo->pos[VY] + (FIX2FLT(P_Random() - 128) * mo->radius / 256),
                        mo->pos[VZ] + deltaZ, P_Random() << 24);
    if(pmo)
    {
        pmo->lastEnemy = mo;
        pmo->mom[MX] = mo->mom[MX];
        pmo->mom[MY] = mo->mom[MY];
        pmo->target = mo->target;
        if(mo->type == MT_LIGHTNING_FLOOR)
        {
            pmo->mom[MZ] = 20;
        }
        else
        {
            pmo->mom[MZ] = -20;
        }
    }

    if(mo->type == MT_LIGHTNING_FLOOR && P_Random() < 160)
    {
        S_StartSound(SFX_MAGE_LIGHTNING_CONTINUOUS, mo);
    }
}

void C_DECL A_MLightningAttack2(mobj_t *mo)
{
    mobj_t         *fmo, *cmo;

    fmo = P_SpawnPlayerMissile(MT_LIGHTNING_FLOOR, mo);
    cmo = P_SpawnPlayerMissile(MT_LIGHTNING_CEILING, mo);
    if(fmo)
    {
        fmo->special1 = 0;
        fmo->lastEnemy = cmo;
        A_LightningZap(fmo);
    }

    if(cmo)
    {
        cmo->tracer = NULL; // Mobj that it will track.
        cmo->lastEnemy = fmo;
        A_LightningZap(cmo);
    }
    S_StartSound(SFX_MAGE_LIGHTNING_FIRE, mo);
}

void C_DECL A_MLightningAttack(player_t *plr, pspdef_t *psp)
{
    A_MLightningAttack2(plr->plr->mo);
    P_ShotAmmo(plr);
}

void C_DECL A_ZapMimic(mobj_t* mo)
{
    mobj_t*             target;

    target = mo->lastEnemy;
    if(target)
    {
        if(target->state >= &STATES[P_GetState(target->type, SN_DEATH)] ||
           target->state == &STATES[S_FREETARGMOBJ])
        {
            P_ExplodeMissile(mo);
        }
        else
        {
            mo->mom[MX] = target->mom[MX];
            mo->mom[MY] = target->mom[MY];
        }
    }
}

void C_DECL A_LastZap(mobj_t* mo)
{
    mobj_t*             pmo;

    pmo = P_SpawnMobj3fv(MT_LIGHTNING_ZAP, mo->pos, P_Random() << 24);
    if(pmo)
    {
        P_MobjChangeState(pmo, S_LIGHTNING_ZAP_X1);
        pmo->mom[MZ] = 40;
    }
}

void C_DECL A_LightningRemove(mobj_t *mo)
{
    mobj_t         *target;

    target = mo->lastEnemy;
    if(target)
    {
        target->lastEnemy = NULL;
        P_ExplodeMissile(target);
    }
}

void MStaffSpawn(mobj_t *mo, angle_t angle)
{
    mobj_t         *pmo;

    pmo = P_SPMAngle(MT_MSTAFF_FX2, mo, angle);
    if(pmo)
    {
        pmo->target = mo;
        pmo->tracer = P_RoughMonsterSearch(pmo, 10*128);
    }
}

void C_DECL A_MStaffAttack(player_t* plr, pspdef_t* psp)
{
    angle_t         angle;
    mobj_t*         mo;

    P_ShotAmmo(plr);
    mo = plr->plr->mo;
    angle = mo->angle;

    MStaffSpawn(mo, angle);
    MStaffSpawn(mo, angle - ANGLE_1 * 5);
    MStaffSpawn(mo, angle + ANGLE_1 * 5);
    S_StartSound(SFX_MAGE_STAFF_FIRE, plr->plr->mo);
    plr->damageCount = 0;
    plr->bonusCount = 0;

    if(plr == &players[CONSOLEPLAYER])
    {
        int             pal = STARTSCOURGEPAL;
        float           rgba[4];

        // $democam
        R_GetFilterColor(rgba, pal);
        GL_SetFilterColor(rgba[CR], rgba[CG], rgba[CB], rgba[CA]);

        GL_SetFilter(true);
    }
}

void C_DECL A_MStaffPalette(player_t* plr, pspdef_t* psp)
{
    if(plr == &players[CONSOLEPLAYER])
    {
        int             pal = STARTSCOURGEPAL +
            psp->state - (&STATES[S_MSTAFFATK_2]);

        if(pal == STARTSCOURGEPAL + 3)
        {   // Reset back to original playpal.
            pal = 0;
        }

        if(pal)
        {
            float           rgba[4];

            // $democam
            R_GetFilterColor(rgba, pal);
            GL_SetFilterColor(rgba[CR], rgba[CG], rgba[CB], rgba[CA]);

            GL_SetFilter(true);
        }
    }
}

void C_DECL A_MStaffWeave(mobj_t *mo)
{
    float       pos[2];
    uint        weaveXY, weaveZ;
    uint        an;

    weaveXY = mo->special2 >> 16;
    weaveZ = mo->special2 & 0xFFFF;
    an = (mo->angle + ANG90) >> ANGLETOFINESHIFT;

    pos[VX] = mo->pos[VX];
    pos[VY] = mo->pos[VY];

    pos[VX] -= FIX2FLT(finecosine[an]) * (FLOATBOBOFFSET(weaveXY) * 4);
    pos[VY] -= FIX2FLT(finesine[an]) * (FLOATBOBOFFSET(weaveXY) * 4);

    weaveXY = (weaveXY + 6) & 63;
    pos[VX] += FIX2FLT(finecosine[an]) * (FLOATBOBOFFSET(weaveXY) * 4);
    pos[VY] += FIX2FLT(finesine[an]) * (FLOATBOBOFFSET(weaveXY) * 4);

    P_TryMove(mo, pos[VX], pos[VY]);
    mo->pos[VZ] -= FLOATBOBOFFSET(weaveZ) * 2;
    weaveZ = (weaveZ + 3) & 63;
    mo->pos[VZ] += FLOATBOBOFFSET(weaveZ) * 2;

    if(mo->pos[VZ] <= mo->floorZ)
    {
        mo->pos[VZ] = mo->floorZ + 1;
    }
    mo->special2 = weaveZ + (weaveXY << 16);
}

void C_DECL A_MStaffTrack(mobj_t *mo)
{
    if((mo->tracer == 0) && (P_Random() < 50))
    {
        mo->tracer = P_RoughMonsterSearch(mo, 10*128);
    }
    P_SeekerMissile(mo, ANGLE_1 * 2, ANGLE_1 * 10);
}

/**
 * For use by mage class boss.
 */
void MStaffSpawn2(mobj_t *mo, angle_t angle)
{
    mobj_t         *pmo;

    pmo = P_SpawnMissileAngle(MT_MSTAFF_FX2, mo, angle, 0);
    if(pmo)
    {
        pmo->target = mo;
        pmo->tracer = P_RoughMonsterSearch(pmo, 10*128);
    }
}

/**
 * For use by mage class boss.
 */
void C_DECL A_MStaffAttack2(mobj_t *mo)
{
    angle_t         angle;

    angle = mo->angle;
    MStaffSpawn2(mo, angle);
    MStaffSpawn2(mo, angle - ANGLE_1 * 5);
    MStaffSpawn2(mo, angle + ANGLE_1 * 5);
    S_StartSound(SFX_MAGE_STAFF_FIRE, mo);
}

void C_DECL A_FPunchAttack(player_t *plr, pspdef_t *psp)
{
    int         i;
    angle_t     angle;
    int         damage;
    float       slope;
    mobj_t     *mo = plr->plr->mo;
    float       power;

    damage = 40 + (P_Random() & 15);
    power = 2;
    puffType = MT_PUNCHPUFF;

    for(i = 0; i < 16; ++i)
    {
        angle = mo->angle + i * (ANG45 / 16);
        slope = P_AimLineAttack(mo, angle, 2 * MELEERANGE);
        if(lineTarget)
        {
            plr->plr->mo->special1++;
            if(mo->special1 == 3)
            {
                damage /= 2;
                power = 6;
                puffType = MT_HAMMERPUFF;
            }

            P_LineAttack(mo, angle, 2 * MELEERANGE, slope, damage);
            if((lineTarget->flags & MF_COUNTKILL) || lineTarget->player)
            {
                P_ThrustMobj(lineTarget, angle, power);
            }

            AdjustPlayerAngle(mo);
            goto punchdone;
        }

        angle = mo->angle - i * (ANG45 / 16);
        slope = P_AimLineAttack(mo, angle, 2 * MELEERANGE);
        if(lineTarget)
        {
            mo->special1++;
            if(mo->special1 == 3)
            {
                damage /= 2;
                power = 6;
                puffType = MT_HAMMERPUFF;
            }

            P_LineAttack(mo, angle, 2 * MELEERANGE, slope, damage);
            if((lineTarget->flags & MF_COUNTKILL) || lineTarget->player)
            {
                P_ThrustMobj(lineTarget, angle, power);
            }

            AdjustPlayerAngle(mo);
            goto punchdone;
        }
    }

    // Didn't find any creatures, so try to strike any walls.
    mo->special1 = 0;

    angle = mo->angle;
    slope = P_AimLineAttack(mo, angle, MELEERANGE);
    P_LineAttack(mo, angle, MELEERANGE, slope, damage);

  punchdone:
    if(mo->special1 == 3)
    {
        mo->special1 = 0;
        P_SetPsprite(plr, PS_WEAPON, S_PUNCHATK2_1);
        S_StartSound(SFX_FIGHTER_GRUNT, mo);
    }
}

void C_DECL A_FAxeAttack(player_t *plr, pspdef_t *psp)
{
    int         i;
    angle_t     angle;
    mobj_t     *pmo = plr->plr->mo;
    float       power;
    float       slope;
    int         damage, useMana;

    damage = 40 + (P_Random() & 15) + (P_Random() & 7);
    power = 0;
    if(plr->ammo[AT_BLUEMANA].owned > 0)
    {
        damage /= 2;
        power = 6;
        puffType = MT_AXEPUFF_GLOW;
        useMana = 1;
    }
    else
    {
        puffType = MT_AXEPUFF;
        useMana = 0;
    }

    for(i = 0; i < 16; ++i)
    {
        angle = pmo->angle + i * (ANG45 / 16);
        slope = P_AimLineAttack(pmo, angle, AXERANGE);
        if(lineTarget)
        {
            P_LineAttack(pmo, angle, AXERANGE, slope, damage);
            if((lineTarget->flags & MF_COUNTKILL) || lineTarget->player)
            {
                P_ThrustMobj(lineTarget, angle, power);
            }

            AdjustPlayerAngle(pmo);
            useMana++;
            goto axedone;
        }

        angle = pmo->angle - i * (ANG45 / 16);
        slope = P_AimLineAttack(pmo, angle, AXERANGE);
        if(lineTarget)
        {
            P_LineAttack(pmo, angle, AXERANGE, slope, damage);
            if(lineTarget->flags & MF_COUNTKILL)
            {
                P_ThrustMobj(lineTarget, angle, power);
            }

            AdjustPlayerAngle(pmo);
            useMana++;
            goto axedone;
        }
    }

    // Didn't find any creatures, so try to strike any walls.
    pmo->special1 = 0;

    angle = pmo->angle;
    slope = P_AimLineAttack(pmo, angle, MELEERANGE);
    P_LineAttack(pmo, angle, MELEERANGE, slope, damage);

  axedone:
    if(useMana == 2)
    {
        P_ShotAmmo(plr);
        if(!(plr->ammo[AT_BLUEMANA].owned > 0))
            P_SetPsprite(plr, PS_WEAPON, S_FAXEATK_5);
    }
}

void C_DECL A_CMaceAttack(player_t *plr, pspdef_t *psp)
{
    int         i;
    angle_t     angle;
    int         damage;
    float       slope;

    damage = 25 + (P_Random() & 15);
    puffType = MT_HAMMERPUFF;
    for(i = 0; i < 16; ++i)
    {
        angle = plr->plr->mo->angle + i * (ANG45 / 16);
        slope = P_AimLineAttack(plr->plr->mo, angle, 2 * MELEERANGE);
        if(lineTarget)
        {
            P_LineAttack(plr->plr->mo, angle, 2 * MELEERANGE, slope,
                         damage);
            AdjustPlayerAngle(plr->plr->mo);
            goto macedone;
        }

        angle = plr->plr->mo->angle - i * (ANG45 / 16);
        slope = P_AimLineAttack(plr->plr->mo, angle, 2 * MELEERANGE);
        if(lineTarget)
        {
            P_LineAttack(plr->plr->mo, angle, 2 * MELEERANGE, slope,
                         damage);
            AdjustPlayerAngle(plr->plr->mo);
            goto macedone;
        }
    }

    // Didn't find any creatures, so try to strike any walls.
    plr->plr->mo->special1 = 0;

    angle = plr->plr->mo->angle;
    slope = P_AimLineAttack(plr->plr->mo, angle, MELEERANGE);
    P_LineAttack(plr->plr->mo, angle, MELEERANGE, slope, damage);
  macedone:
    return;
}

void C_DECL A_CStaffCheck(player_t *plr, pspdef_t *psp)
{
    int         i;
    mobj_t     *pmo;
    int         damage, newLife;
    angle_t     angle;
    float       slope;

    pmo = plr->plr->mo;
    damage = 20 + (P_Random() & 15);
    puffType = MT_CSTAFFPUFF;
    for(i = 0; i < 3; ++i)
    {
        angle = pmo->angle + i * (ANG45 / 16);
        slope = P_AimLineAttack(pmo, angle, 1.5 * MELEERANGE);
        if(lineTarget)
        {
            P_LineAttack(pmo, angle, 1.5 * MELEERANGE, slope, damage);

            pmo->angle =
                R_PointToAngle2(pmo->pos[VX], pmo->pos[VY],
                                lineTarget->pos[VX], lineTarget->pos[VY]);

            if((lineTarget->player || (lineTarget->flags & MF_COUNTKILL)) &&
               (!(lineTarget->flags2 & (MF2_DORMANT + MF2_INVULNERABLE))))
            {
                newLife = plr->health + (damage / 8);
                newLife = (newLife > 100 ? 100 : newLife);
                pmo->health = plr->health = newLife;

                P_SetPsprite(plr, PS_WEAPON, S_CSTAFFATK2_1);
            }

            P_ShotAmmo(plr);
            break;
        }

        angle = pmo->angle - i * (ANG45 / 16);
        slope = P_AimLineAttack(plr->plr->mo, angle, 1.5 * MELEERANGE);
        if(lineTarget)
        {
            P_LineAttack(pmo, angle, 1.5 * MELEERANGE, slope, damage);
            pmo->angle =
                R_PointToAngle2(pmo->pos[VX], pmo->pos[VY],
                                lineTarget->pos[VX], lineTarget->pos[VY]);
            if(lineTarget->player || (lineTarget->flags & MF_COUNTKILL))
            {
                newLife = plr->health + (damage >> 4);
                newLife = (newLife > 100 ? 100 : newLife);
                pmo->health = plr->health = newLife;

                P_SetPsprite(plr, PS_WEAPON, S_CSTAFFATK2_1);
            }

            P_ShotAmmo(plr);
            break;
        }
    }
}

void C_DECL A_CStaffAttack(player_t *plr, pspdef_t *psp)
{
    mobj_t     *mo, *pmo;

    P_ShotAmmo(plr);
    pmo = plr->plr->mo;
    mo = P_SPMAngle(MT_CSTAFF_MISSILE, pmo, pmo->angle - (ANG45 / 15));
    if(mo)
    {
        mo->special2 = 32;
    }

    mo = P_SPMAngle(MT_CSTAFF_MISSILE, pmo, pmo->angle + (ANG45 / 15));
    if(mo)
    {
        mo->special2 = 0;
    }

    S_StartSound(SFX_CLERIC_CSTAFF_FIRE, plr->plr->mo);
}

void C_DECL A_CStaffMissileSlither(mobj_t *actor)
{
    float       pos[2];
    uint        an, weaveXY;

    weaveXY = actor->special2;
    an = (actor->angle + ANG90) >> ANGLETOFINESHIFT;

    pos[VX] = actor->pos[VX];
    pos[VY] = actor->pos[VY];

    pos[VX] -= FIX2FLT(finecosine[an]) * FLOATBOBOFFSET(weaveXY);
    pos[VY] -= FIX2FLT(finesine[an]) * FLOATBOBOFFSET(weaveXY);

    weaveXY = (weaveXY + 3) & 63;
    pos[VX] += FIX2FLT(finecosine[an]) * FLOATBOBOFFSET(weaveXY);
    pos[VY] += FIX2FLT(finesine[an]) * FLOATBOBOFFSET(weaveXY);

    P_TryMove(actor, pos[VX], pos[VY]);
    actor->special2 = weaveXY;
}

void C_DECL A_CStaffInitBlink(player_t *plr, pspdef_t *psp)
{
    plr->plr->mo->special1 = (P_Random() >> 1) + 20;
}

void C_DECL A_CStaffCheckBlink(player_t *plr, pspdef_t *psp)
{
    if(!--plr->plr->mo->special1)
    {
        P_SetPsprite(plr, PS_WEAPON, S_CSTAFFBLINK1);
        plr->plr->mo->special1 = (P_Random() + 50) >> 2;
    }
}

void C_DECL A_CFlameAttack(player_t *plr, pspdef_t *psp)
{
    mobj_t         *pmo;

    pmo = P_SpawnPlayerMissile(MT_CFLAME_MISSILE, plr->plr->mo);
    if(pmo)
    {
        pmo->special1 = 2;
    }

    P_ShotAmmo(plr);
    S_StartSound(SFX_CLERIC_FLAME_FIRE, plr->plr->mo);
}

void C_DECL A_CFlamePuff(mobj_t *mo)
{
    A_UnHideThing(mo);
    mo->mom[MX] = mo->mom[MY] = mo->mom[MZ] = 0;
    S_StartSound(SFX_CLERIC_FLAME_EXPLODE, mo);
}

void C_DECL A_CFlameMissile(mobj_t *mo)
{
    int         i;
    uint        an, an90;
    float       dist;
    mobj_t     *pmo;

    A_UnHideThing(mo);
    S_StartSound(SFX_CLERIC_FLAME_EXPLODE, mo);

    if(blockingMobj && (blockingMobj->flags & MF_SHOOTABLE))
    {   // Hit something.
        // Spawn the flame circle around the thing
        dist = blockingMobj->radius + 18;
        for(i = 0; i < 4; ++i)
        {
            an = (i * ANG45) >> ANGLETOFINESHIFT;
            an90 = (i * ANG45 + ANG90) >> ANGLETOFINESHIFT;
            pmo = P_SpawnMobj3f(MT_CIRCLEFLAME,
                                blockingMobj->pos[VX] + dist * FIX2FLT(finecosine[an]),
                                blockingMobj->pos[VY] + dist * FIX2FLT(finesine[an]),
                                blockingMobj->pos[VZ] + 5,
                                (angle_t) an << ANGLETOFINESHIFT);
            if(pmo)
            {
                pmo->target = mo->target;
                pmo->mom[MX] = FLAMESPEED * FIX2FLT(finecosine[an]);
                pmo->mom[MY] = FLAMESPEED * FIX2FLT(finesine[an]);

                pmo->special1 = FLT2FIX(pmo->mom[MX]);
                pmo->special2 = FLT2FIX(pmo->mom[MY]);
                pmo->tics -= P_Random() & 3;
            }

            pmo = P_SpawnMobj3f(MT_CIRCLEFLAME,
                                blockingMobj->pos[VX] - dist * FIX2FLT(finecosine[an]),
                                blockingMobj->pos[VY] - dist * FIX2FLT(finesine[an]),
                                blockingMobj->pos[VZ] + 5,
                                (angle_t) (ANG180 + (an << ANGLETOFINESHIFT)));
            if(pmo)
            {
                pmo->target = mo->target;
                pmo->mom[MX] = -FLAMESPEED * FIX2FLT(finecosine[an]);
                pmo->mom[MY] = -FLAMESPEED * FIX2FLT(finesine[an]);

                pmo->special1 = FLT2FIX(pmo->mom[MX]);
                pmo->special2 = FLT2FIX(pmo->mom[MY]);
                pmo->tics -= P_Random() & 3;
            }
        }

        P_MobjChangeState(mo, S_FLAMEPUFF2_1);
    }
}

void C_DECL A_CFlameRotate(mobj_t *mo)
{
    uint        an;

    an = (mo->angle + ANG90) >> ANGLETOFINESHIFT;
    mo->mom[MX] = FIX2FLT(mo->special1);
    mo->mom[MY] = FIX2FLT(mo->special2);
    mo->mom[MX] += FLAMEROTSPEED * FIX2FLT(finecosine[an]);
    mo->mom[MY] += FLAMEROTSPEED * FIX2FLT(finesine[an]);
    mo->angle += ANG90 / 15;
}

/**
 * Spawns the spirits
 */
void C_DECL A_CHolyAttack3(mobj_t *mo)
{
    P_SpawnMissile(MT_HOLY_MISSILE, mo, mo->target);
    S_StartSound(SFX_CHOLY_FIRE, mo);
}

/**
 * Spawns the spirits
 */
void C_DECL A_CHolyAttack2(mobj_t *mo)
{
    int         i, j;
    mobj_t     *pmo, *tail, *next;

    for(i = 0; i < 4; ++i)
    {
        pmo = P_SpawnMobj3fv(MT_HOLY_FX, mo->pos,
                             mo->angle + (ANGLE_45 + ANGLE_45 / 2) - ANGLE_45 * i);
        if(!pmo)
            continue;

        switch(i) // float bob index
        {
        case 0:
            pmo->special2 = P_Random() & 7; // Upper-left.
            break;

        case 1:
            pmo->special2 = 32 + (P_Random() & 7); // Upper-right.
            break;

        case 2:
            pmo->special2 = (32 + (P_Random() & 7)) << 16; // Lower-left.
            break;

        case 3:
            pmo->special2 =
                ((32 + (P_Random() & 7)) << 16) + 32 + (P_Random() & 7);
            break;
        }

        pmo->pos[VZ] = mo->pos[VZ];
        P_ThrustMobj(pmo, pmo->angle, pmo->info->speed);
        pmo->target = mo->target;
        pmo->args[0] = 10; // Initial turn value.
        pmo->args[1] = 0; // Initial look angle.
        if(deathmatch)
        {   // Ghosts last slightly less longer in DeathMatch.
            pmo->health = 85;
        }

        if(lineTarget)
        {
            pmo->tracer = lineTarget;
            pmo->flags |= MF_NOCLIP | MF_SKULLFLY;
            pmo->flags &= ~MF_MISSILE;
        }

        tail = P_SpawnMobj3fv(MT_HOLY_TAIL, pmo->pos, pmo->angle + ANG180);
        tail->target = pmo; // Parent.
        for(j = 1; j < 3; ++j)
        {
            next = P_SpawnMobj3fv(MT_HOLY_TAIL, pmo->pos,
                                  pmo->angle + ANG180);
            P_MobjChangeState(next, P_GetState(next->type, SN_SPAWN) + 1);
            tail->tracer = next;
            tail = next;
        }

        tail->tracer = NULL;     // last tail bit
    }
}

void C_DECL A_CHolyAttack(player_t* plr, pspdef_t* psp)
{
    mobj_t*             pmo;

    P_ShotAmmo(plr);
    pmo = P_SpawnPlayerMissile(MT_HOLY_MISSILE, plr->plr->mo);
    plr->damageCount = 0;
    plr->bonusCount = 0;

    if(plr == &players[CONSOLEPLAYER])
    {
        float               rgba[4];

        // $democam
        R_GetFilterColor(rgba, STARTHOLYPAL);
        GL_SetFilterColor(rgba[CR], rgba[CG], rgba[CB], rgba[CA]);

        GL_SetFilter(true);
    }

    S_StartSound(SFX_CHOLY_FIRE, plr->plr->mo);
}

void C_DECL A_CHolyPalette(player_t* plr, pspdef_t* psp)
{
    if(plr == &players[CONSOLEPLAYER])
    {
        int                 pal = STARTHOLYPAL +
            psp->state - (&STATES[S_CHOLYATK_6]);

        if(pal == STARTHOLYPAL + 3)
        {   // reset back to original playpal
            pal = 0;
        }

        if(pal)
        {
            float               rgba[4];

            // $democam
            R_GetFilterColor(rgba, pal);
            GL_SetFilterColor(rgba[CR], rgba[CG], rgba[CB], rgba[CA]);

            GL_SetFilter(true);
        }
    }
}

static void CHolyFindTarget(mobj_t* mo)
{
    mobj_t*             target;

    target = P_RoughMonsterSearch(mo, 6*128);
    if(target)
    {
        Con_Message("CHolyFindTarget: mobj_t* converted to int! Not 64-bit compatible.\n");
        mo->tracer = target;
        mo->flags |= MF_NOCLIP | MF_SKULLFLY;
        mo->flags &= ~MF_MISSILE;
    }
}

/**
 * Similar to P_SeekerMissile, but seeks to a random Z on the target
 */
static void CHolySeekerMissile(mobj_t *mo, angle_t thresh, angle_t turnMax)
{
    int         dir;
    uint        an;
    angle_t     delta;
    mobj_t     *target;
    float       dist, newZ, deltaZ;

    target = mo->tracer;
    if(target == NULL)
    {
        return;
    }

    if(!(target->flags & MF_SHOOTABLE) ||
       (!(target->flags & MF_COUNTKILL) && !target->player))
    {   // Target died/target isn't a player or creature
        mo->tracer = NULL;
        mo->flags &= ~(MF_NOCLIP | MF_SKULLFLY);
        mo->flags |= MF_MISSILE;
        CHolyFindTarget(mo);
        return;
    }

    dir = P_FaceMobj(mo, target, &delta);
    if(delta > thresh)
    {
        delta /= 2;
        if(delta > turnMax)
        {
            delta = turnMax;
        }
    }

    if(dir)
    {   // Turn clockwise
        mo->angle += delta;
    }
    else
    {   // Turn counter clockwise
        mo->angle -= delta;
    }

    an = mo->angle >> ANGLETOFINESHIFT;
    mo->mom[MX] = mo->info->speed * FIX2FLT(finecosine[an]);
    mo->mom[MY] = mo->info->speed * FIX2FLT(finesine[an]);

    if(!(mapTime & 15) ||
       mo->pos[VZ] > target->pos[VZ] + target->height ||
       mo->pos[VZ] + mo->height < target->pos[VZ])
    {
        newZ = target->pos[VZ];
        newZ += FIX2FLT((P_Random() * FLT2FIX(target->height)) >> 8);
        deltaZ = newZ - mo->pos[VZ];

        if(fabs(deltaZ) > 15)
        {
            if(deltaZ > 0)
            {
                deltaZ = 15;
            }
            else
            {
                deltaZ = -15;
            }
        }

        dist = P_ApproxDistance(target->pos[VX] - mo->pos[VX],
                                target->pos[VX] - mo->pos[VY]);
        dist /= mo->info->speed;
        if(dist < 1)
        {
            dist = 1;
        }
        mo->mom[MZ] = deltaZ / dist;
    }
}

static void CHolyWeave(mobj_t *mo)
{
    float       pos[2];
    int         weaveXY, weaveZ;
    int         angle;

    weaveXY = mo->special2 >> 16;
    weaveZ = mo->special2 & 0xFFFF;
    angle = (mo->angle + ANG90) >> ANGLETOFINESHIFT;

    pos[VX] = mo->pos[VX] - (FIX2FLT(finecosine[angle]) * (FLOATBOBOFFSET(weaveXY) * 4));
    pos[VY] = mo->pos[VY] - (FIX2FLT(finesine[angle]) * (FLOATBOBOFFSET(weaveXY) * 4));

    weaveXY = (weaveXY + (P_Random() % 5)) & 63;
    pos[VX] += FIX2FLT(finecosine[angle]) * (FLOATBOBOFFSET(weaveXY) * 4);
    pos[VY] += FIX2FLT(finesine[angle]) * (FLOATBOBOFFSET(weaveXY) * 4);

    P_TryMove(mo, pos[VX], pos[VY]);
    mo->pos[VZ] -= FLOATBOBOFFSET(weaveZ) * 2;
    weaveZ = (weaveZ + (P_Random() % 5)) & 63;
    mo->pos[VZ] += FLOATBOBOFFSET(weaveZ) * 2;

    mo->special2 = weaveZ + (weaveXY << 16);
}

void C_DECL A_CHolySeek(mobj_t* mo)
{
    mo->health--;
    if(mo->health <= 0)
    {
        mo->mom[MX] /= 4;
        mo->mom[MY] /= 4;
        mo->mom[MZ] = 0;
        P_MobjChangeState(mo, P_GetState(mo->type, SN_DEATH));
        mo->tics -= P_Random() & 3;
        return;
    }

    if(mo->tracer)
    {
        CHolySeekerMissile(mo, mo->args[0] * ANGLE_1,
                           mo->args[0] * ANGLE_1 * 2);
        if(!((mapTime + 7) & 15))
        {
            mo->args[0] = 5 + (P_Random() / 20);
        }
    }
    CHolyWeave(mo);
}

static void CHolyTailFollow(mobj_t* mo, float dist)
{
    uint                an;
    angle_t             angle;
    mobj_t*             child;
    float               oldDistance, newDistance;

    child = mo->tracer;
    if(child)
    {
        angle = R_PointToAngle2(mo->pos[VX], mo->pos[VY],
                                child->pos[VX], child->pos[VY]);
        an = angle >> ANGLETOFINESHIFT;
        oldDistance =
            P_ApproxDistance(child->pos[VX] - mo->pos[VX],
                             child->pos[VY] - mo->pos[VY]);
        if(P_TryMove(child,
                     mo->pos[VX] + dist * FIX2FLT(finecosine[an]),
                     mo->pos[VY] + dist * FIX2FLT(finesine[an])))
        {
            newDistance =
                P_ApproxDistance(child->pos[VX] - mo->pos[VX],
                                 child->pos[VY] - mo->pos[VY]) - 1;
            if(oldDistance < 1)
            {
                if(child->pos[VZ] < mo->pos[VZ])
                {
                    child->pos[VZ] = mo->pos[VZ] - dist;
                }
                else
                {
                    child->pos[VZ] = mo->pos[VZ] + dist;
                }
            }
            else
            {
                child->pos[VZ] = mo->pos[VZ] +
                    (newDistance / oldDistance) * (child->pos[VZ] - mo->pos[VZ]);
            }
        }
        CHolyTailFollow(child, dist - 1);
    }
}

static void CHolyTailRemove(mobj_t *mo)
{
    mobj_t     *child;

    child = mo->tracer;
    if(child)
    {
        CHolyTailRemove(child);
    }

    P_MobjRemove(mo, false);
}

void C_DECL A_CHolyTail(mobj_t *mo)
{
    mobj_t     *parent;

    parent = mo->target;
    if(parent)
    {
        if(parent->state >= &STATES[P_GetState(parent->type, SN_DEATH)])
        {   // Ghost removed, so remove all tail parts.
            CHolyTailRemove(mo);
        }
        else
        {
            uint        an = parent->angle >> ANGLETOFINESHIFT;

            if(P_TryMove(mo,
                         parent->pos[VX] - (14 * FIX2FLT(finecosine[an])),
                         parent->pos[VY] - (14 * FIX2FLT(finesine[an]))))
            {
                mo->pos[VZ] = parent->pos[VZ] - 5;
            }

            CHolyTailFollow(mo, 10);
        }
    }
}

void C_DECL A_CHolyCheckScream(mobj_t *mo)
{
    A_CHolySeek(mo);
    if(P_Random() < 20)
    {
        S_StartSound(SFX_SPIRIT_ACTIVE, mo);
    }

    if(!mo->tracer)
    {
        CHolyFindTarget(mo);
    }
}

void C_DECL A_CHolySpawnPuff(mobj_t *mo)
{
    P_SpawnMobj3fv(MT_HOLY_MISSILE_PUFF, mo->pos, P_Random() << 24);
}

void C_DECL A_FireConePL1(player_t *plr, pspdef_t *psp)
{
    int         i, damage;
    angle_t     angle;
    mobj_t     *pmo, *mo;
    boolean     conedone = false;

    mo = plr->plr->mo;
    P_ShotAmmo(plr);
    S_StartSound(SFX_MAGE_SHARDS_FIRE, mo);

    damage = 90 + (P_Random() & 15);
    for(i = 0; i < 16; ++i)
    {
        angle = mo->angle + i * (ANG45 / 16);

        P_AimLineAttack(mo, angle, MELEERANGE);
        if(lineTarget)
        {
            mo->flags2 |= MF2_ICEDAMAGE;
            P_DamageMobj(lineTarget, mo, mo, damage, false);
            mo->flags2 &= ~MF2_ICEDAMAGE;
            conedone = true;
            break;
        }
    }

    // Didn't find any creatures, so fire projectiles.
    if(!conedone)
    {
        pmo = P_SpawnPlayerMissile(MT_SHARDFX1, mo);
        if(pmo)
        {
            pmo->special1 =
                SHARDSPAWN_LEFT | SHARDSPAWN_DOWN | SHARDSPAWN_UP |
                SHARDSPAWN_RIGHT;
            pmo->special2 = 3; // Set sperm count (levels of reproductivity)
            pmo->target = mo;
            pmo->args[0] = 3; // Mark Initial shard as super damage
        }
    }
}

void C_DECL A_ShedShard(mobj_t *mo)
{
    mobj_t     *pmo;
    int         spawndir = mo->special1;
    int         spermcount = mo->special2;

    if(spermcount <= 0)
        return; // No sperm left, can no longer reproduce.

    mo->special2 = 0;
    spermcount--;

    // Every so many calls, spawn a new missile in it's set directions.
    if(spawndir & SHARDSPAWN_LEFT)
    {
        pmo = P_SpawnMissileAngleSpeed(MT_SHARDFX1, mo,
                                       mo->angle + (ANG45 / 9), 0,
                                       (20 + 2 * spermcount));
        if(pmo)
        {
            pmo->special1 = SHARDSPAWN_LEFT;
            pmo->special2 = spermcount;
            pmo->mom[MZ] = mo->mom[MZ];
            pmo->target = mo->target;
            pmo->args[0] = (spermcount == 3) ? 2 : 0;
        }
    }

    if(spawndir & SHARDSPAWN_RIGHT)
    {
        pmo = P_SpawnMissileAngleSpeed(MT_SHARDFX1, mo,
                                       mo->angle - (ANG45 / 9), 0,
                                       (20 + 2 * spermcount));
        if(pmo)
        {
            pmo->special1 = SHARDSPAWN_RIGHT;
            pmo->special2 = spermcount;
            pmo->mom[MZ] = mo->mom[MZ];
            pmo->target = mo->target;
            pmo->args[0] = (spermcount == 3) ? 2 : 0;
        }
    }

    if(spawndir & SHARDSPAWN_UP)
    {
        pmo = P_SpawnMissileAngleSpeed(MT_SHARDFX1, mo, mo->angle, 0,
                                      (15 + 2 * spermcount));
        if(pmo)
        {
            pmo->mom[MZ] = mo->mom[MZ];
            pmo->pos[VZ] += 8;
            if(spermcount & 1) // Every other reproduction.
                pmo->special1 =
                    SHARDSPAWN_UP | SHARDSPAWN_LEFT | SHARDSPAWN_RIGHT;
            else
                pmo->special1 = SHARDSPAWN_UP;
            pmo->special2 = spermcount;
            pmo->target = mo->target;
            pmo->args[0] = (spermcount == 3) ? 2 : 0;
        }
    }

    if(spawndir & SHARDSPAWN_DOWN)
    {
        pmo = P_SpawnMissileAngleSpeed(MT_SHARDFX1, mo, mo->angle, 0,
                                      (15 + 2 * spermcount));
        if(pmo)
        {
            pmo->mom[MZ] = mo->mom[MZ];
            pmo->pos[VZ] -= 4;
            if(spermcount & 1) // Every other reproduction.
                pmo->special1 =
                    SHARDSPAWN_DOWN | SHARDSPAWN_LEFT | SHARDSPAWN_RIGHT;
            else
                pmo->special1 = SHARDSPAWN_DOWN;
            pmo->special2 = spermcount;
            pmo->target = mo->target;
            pmo->args[0] = (spermcount == 3) ? 2 : 0;
        }
    }
}

void C_DECL A_Light0(player_t *plr, pspdef_t *psp)
{
    plr->plr->extraLight = 0;
}

/**
 * Called at start of the map for each player.
 */
void P_SetupPsprites(player_t *plr)
{
    int                 i;

#if _DEBUG
    Con_Message("P_SetupPsprites: Player %i.\n", plr - players);
#endif

    // Remove all psprites.
    for(i = 0; i < NUMPSPRITES; ++i)
    {
        plr->pSprites[i].state = NULL;
    }

    // Spawn the ready weapon
    plr->pendingWeapon = plr->readyWeapon;
    P_BringUpWeapon(plr);
}

/**
 * Called every tic by player thinking routine.
 */
void P_MovePsprites(player_t *plr)
{
    int         i;
    pspdef_t   *psp;
    state_t    *state;

    psp = &plr->pSprites[0];
    for(i = 0; i < NUMPSPRITES; ++i, psp++)
    {
        if((state = psp->state) != 0) // A null state means not active.
        {
            // Drop tic count and possibly change state.
            if(psp->tics != -1) // A -1 tic count never changes.
            {
                psp->tics--;
                if(!psp->tics)
                {
                    P_SetPsprite(plr, i, psp->state->nextState);
                }
            }
        }
    }

    plr->pSprites[PS_FLASH].pos[VX] = plr->pSprites[PS_WEAPON].pos[VX];
    plr->pSprites[PS_FLASH].pos[VY] = plr->pSprites[PS_WEAPON].pos[VY];
}

boolean P_UseArtiPoisonBag(player_t* player)
{
    mobj_t*         plrmo, *mo;
    float           pos[3];
    angle_t         angle;
    mobjtype_t      type;

    if(!player)
        return false;

    plrmo = player->plr->mo;

    if(player->pClass == PCLASS_FIGHTER || player->pClass == PCLASS_PIG)
    {
        type = MT_THROWINGBOMB;
        pos[VX] = plrmo->pos[VX];
        pos[VY] = plrmo->pos[VY];
        pos[VZ] = plrmo->pos[VZ] - plrmo->floorClip + 35;
        angle = plrmo->angle + (((P_Random() & 7) - 4) << 24);
    }
    else
    {
        uint                an = plrmo->angle >> ANGLETOFINESHIFT;

        if(player->pClass == PCLASS_CLERIC)
            type = MT_POISONBAG;
        else
            type = MT_FIREBOMB;
        pos[VX] = plrmo->pos[VX] + 16 * FIX2FLT(finecosine[an]);
        pos[VY] = plrmo->pos[VY] + 24 * FIX2FLT(finesine[an]);
        pos[VZ] = plrmo->pos[VZ] - plrmo->floorClip + 8;
        angle = plrmo->angle;
    }

    mo = P_SpawnMobj3fv(type, pos, angle);
    if(mo)
    {
        mo->target = plrmo;

        if(type == MT_THROWINGBOMB)
        {
            mo->mom[MZ] =
                4 + FIX2FLT(((int) player->plr->lookDir) << (FRACBITS - 4));
            mo->pos[VZ] += FIX2FLT(((int) player->plr->lookDir) << (FRACBITS - 4));

            P_ThrustMobj(mo, mo->angle, mo->info->speed);

            mo->mom[MX] += plrmo->mom[MX] / 2;
            mo->mom[MY] += plrmo->mom[MY] / 2;

            mo->tics -= P_Random() & 3;
            P_CheckMissileSpawn(mo);
        }
    }

    return true;
}

boolean P_UseArtiEgg(player_t* player)
{
    mobj_t*         plrmo;

    if(!player)
        return false;
    plrmo = player->plr->mo;

    P_SpawnPlayerMissile(MT_EGGFX, plrmo);
    P_SPMAngle(MT_EGGFX, plrmo, plrmo->angle - (ANG45 / 6));
    P_SPMAngle(MT_EGGFX, plrmo, plrmo->angle + (ANG45 / 6));
    P_SPMAngle(MT_EGGFX, plrmo, plrmo->angle - (ANG45 / 3));
    P_SPMAngle(MT_EGGFX, plrmo, plrmo->angle + (ANG45 / 3));

    return true;
}

boolean P_UseArtiSummon(player_t* player)
{
    mobj_t*         plrmo, *mo;

    if(!player)
        return false;
    plrmo = player->plr->mo;

    mo = P_SpawnPlayerMissile(MT_SUMMON_FX, plrmo);
    if(mo)
    {
        mo->target = plrmo;
        mo->tracer = plrmo;
        mo->mom[MZ] = 5;
    }

    return true;
}

boolean P_UseArtiBoostArmor(player_t* player)
{
    int             i, count;

    if(!player)
        return false;

    count = 0;
    for(i = 0; i < NUMARMOR; ++i)
    {
        count += P_GiveArmor(player, i, 1); // 1 point per armor type.
    }

    if(!count)
        return false;

    return true;
}

boolean P_UseArtiBoostMana(player_t* player)
{
    if(!player)
        return false;

    if(!P_GiveMana(player, AT_BLUEMANA, MAX_MANA))
    {
        if(!P_GiveMana(player, AT_GREENMANA, MAX_MANA))
        {
            return false;
        }
    }
    else
    {
        P_GiveMana(player, AT_GREENMANA, MAX_MANA);
    }

    return true;
}

boolean P_UseArtiTeleportOther(player_t* player)
{
    if(!player)
        return false;

    P_ArtiTeleportOther(player);

    return true;
}

boolean P_UseArtiSpeed(player_t* player)
{
    if(!player)
        return false;

    return P_GivePower(player, PT_SPEED);
}

boolean P_UseArtiFly(player_t* player)
{
    if(!player)
        return false;

    if(!P_GivePower(player, PT_FLIGHT))
    {
        return false;
    }

    if(player->plr->mo->mom[MZ] <= -35)
    {   // Stop falling scream.
        S_StopSound(0, player->plr->mo);
    }

    return true;
}

boolean P_UseArtiBlastRadius(player_t* player)
{
    if(!player)
        return false;

    P_BlastRadius(player);
    return true;
}

boolean P_UseArtiTeleport(player_t* player)
{
    if(!player)
        return false;

    P_ArtiTele(player);
    return true;
}

boolean P_UseArtiTorch(player_t* player)
{
    if(!player)
        return false;

    return P_GivePower(player, PT_INFRARED);
}

boolean P_UseArtiHealRadius(player_t* player)
{
    if(!player)
        return false;

    return P_HealRadius(player);
}

boolean P_UseArtiHealth(player_t* player)
{
    if(!player)
        return false;

    return P_GiveBody(player, 25);
}

boolean P_UseArtiSuperHealth(player_t* player)
{
    if(!player)
        return false;

    return P_GiveBody(player, 100);
}

boolean P_UseArtiInvulnerability(player_t* player)
{
    if(!player)
        return false;

    return P_GivePower(player, PT_INVULNERABILITY);
}

boolean P_UseArtiPuzzSkull(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZSKULL))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGemBig(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEMBIG))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGemRed(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEMRED))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGemGreen1(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEMGREEN1))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGemGreen2(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEMGREEN2))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGemBlue1(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEMBLUE1))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGemBlue2(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEMBLUE2))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzBook1(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZBOOK1))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzBook2(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZBOOK2))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzSkull2(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZSKULL2))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzFWeapon(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZFWEAPON))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzCWeapon(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZCWEAPON))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzMWeapon(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZMWEAPON))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGear1(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEAR1))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGear2(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEAR2))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGear3(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEAR3))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}

boolean P_UseArtiPuzzGear4(player_t* player)
{
    if(!player)
        return false;

    if(!P_UsePuzzleItem(player, AFT_PUZZGEAR4))
    {
        P_SetYellowMessage(player, TXT_USEPUZZLEFAILED, false);
        return false;
    }

    return true;
}
