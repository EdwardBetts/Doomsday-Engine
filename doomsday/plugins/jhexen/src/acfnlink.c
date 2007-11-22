/**\file
 *\section License
 * License: GPL + jHeretic/jHexen Exception
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright © 2003-2007 Jaakko Keränen <jaakko.keranen@iki.fi>
 *\author Copyright © 2006-2007 Daniel Swanson <danij@dengine.net>
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
 *
 * In addition, as a special exception, we, the authors of deng
 * give permission to link the code of our release of deng with
 * the libjhexen and/or the libjheretic libraries (or with modified
 * versions of it that use the same license as the libjhexen or
 * libjheretic libraries), and distribute the linked executables.
 * You must obey the GNU General Public License in all respects for
 * all of the code used other than “libjhexen or libjheretic”. If
 * you modify this file, you may extend this exception to your
 * version of the file, but you are not obligated to do so. If you
 * do not wish to do so, delete this exception statement from your version.
 */

/**
 * acfnlink.c:
 */

// HEADER FILES ------------------------------------------------------------

#include "jhexen.h"

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

actionlink_t actionlinks[] = {
    {"A_AddPlayerCorpse", A_AddPlayerCorpse},
    {"A_BatMove", A_BatMove},
    {"A_BatSpawn", A_BatSpawn},
    {"A_BatSpawnInit", A_BatSpawnInit},
    {"A_BellReset1", A_BellReset1},
    {"A_BellReset2", A_BellReset2},
    {"A_BishopAttack", A_BishopAttack},
    {"A_BishopAttack2", A_BishopAttack2},
    {"A_BishopChase", A_BishopChase},
    {"A_BishopDecide", A_BishopDecide},
    {"A_BishopDoBlur", A_BishopDoBlur},
    {"A_BishopMissileSeek", A_BishopMissileSeek},
    {"A_BishopMissileWeave", A_BishopMissileWeave},
    {"A_BishopPainBlur", A_BishopPainBlur},
    {"A_BishopPuff", A_BishopPuff},
    {"A_BishopSpawnBlur", A_BishopSpawnBlur},
    {"A_BounceCheck", A_BounceCheck},
    {"A_BridgeInit", A_BridgeInit},
    {"A_BridgeOrbit", A_BridgeOrbit},
    {"A_CentaurAttack", A_CentaurAttack},
    {"A_CentaurAttack2", A_CentaurAttack2},
    {"A_CentaurDefend", A_CentaurDefend},
    {"A_CentaurDropStuff", A_CentaurDropStuff},
    {"A_CFlameAttack", A_CFlameAttack},
    {"A_CFlameMissile", A_CFlameMissile},
    {"A_CFlamePuff", A_CFlamePuff},
    {"A_CFlameRotate", A_CFlameRotate},
    {"A_Chase", A_Chase},
    {"A_CheckBurnGone", A_CheckBurnGone},
    {"A_CheckFloor", A_CheckFloor},
    {"A_CheckSkullDone", A_CheckSkullDone},
    {"A_CheckSkullFloor", A_CheckSkullFloor},
    {"A_CheckTeleRing", A_CheckTeleRing},
    {"A_CheckThrowBomb", A_CheckThrowBomb},
    {"A_CHolyAttack", A_CHolyAttack},
    {"A_CHolyAttack2", A_CHolyAttack2},
    {"A_CHolyCheckScream", A_CHolyCheckScream},
    {"A_CHolyPalette", A_CHolyPalette},
    {"A_CHolySeek", A_CHolySeek},
    {"A_CHolySpawnPuff", A_CHolySpawnPuff},
    {"A_CHolyTail", A_CHolyTail},
    {"A_ClassBossHealth", A_ClassBossHealth},
    {"A_ClericAttack", A_ClericAttack},
    {"A_CMaceAttack", A_CMaceAttack},
    {"A_ContMobjSound", A_ContMobjSound},
    {"A_CorpseBloodDrip", A_CorpseBloodDrip},
    {"A_CorpseExplode", A_CorpseExplode},
    {"A_CStaffAttack", A_CStaffAttack},
    {"A_CStaffCheck", A_CStaffCheck},
    {"A_CStaffCheckBlink", A_CStaffCheckBlink},
    {"A_CStaffInitBlink", A_CStaffInitBlink},
    {"A_CStaffMissileSlither", A_CStaffMissileSlither},
    {"A_DelayGib", A_DelayGib},
    {"A_Demon2Death", A_Demon2Death},
    {"A_DemonAttack1", A_DemonAttack1},
    {"A_DemonAttack2", A_DemonAttack2},
    {"A_DemonDeath", A_DemonDeath},
    {"A_DragonAttack", A_DragonAttack},
    {"A_DragonCheckCrash", A_DragonCheckCrash},
    {"A_DragonFlap", A_DragonFlap},
    {"A_DragonFlight", A_DragonFlight},
    {"A_DragonFX2", A_DragonFX2},
    {"A_DragonInitFlight", A_DragonInitFlight},
    {"A_DragonPain", A_DragonPain},
    {"A_DropMace", A_DropMace},
    {"A_ESound", A_ESound},
    {"A_EttinAttack", A_EttinAttack},
    {"A_Explode", A_Explode},
    {"A_FaceTarget", A_FaceTarget},
    {"A_FastChase", A_FastChase},
    {"A_FAxeAttack", A_FAxeAttack},
    {"A_FHammerAttack", A_FHammerAttack},
    {"A_FHammerThrow", A_FHammerThrow},
    {"A_FighterAttack", A_FighterAttack},
    {"A_FireConePL1", A_FireConePL1},
    {"A_FiredAttack", A_FiredAttack},
    {"A_FiredChase", A_FiredChase},
    {"A_FiredRocks", A_FiredRocks},
    {"A_FiredSplotch", A_FiredSplotch},
    {"A_FlameCheck", A_FlameCheck},
    {"A_FloatGib", A_FloatGib},
    {"A_FogMove", A_FogMove},
    {"A_FogSpawn", A_FogSpawn},
    {"A_FPunchAttack", A_FPunchAttack},
    {"A_FreeTargMobj", A_FreeTargMobj},
    {"A_FreezeDeath", A_FreezeDeath},
    {"A_FreezeDeathChunks", A_FreezeDeathChunks},
    {"A_FSwordAttack", A_FSwordAttack},
    {"A_FSwordFlames", A_FSwordFlames},
    {"A_HideThing", A_HideThing},
    {"A_IceCheckHeadDone", A_IceCheckHeadDone},
    {"A_IceGuyAttack", A_IceGuyAttack},
    {"A_IceGuyChase", A_IceGuyChase},
    {"A_IceGuyDie", A_IceGuyDie},
    {"A_IceGuyLook", A_IceGuyLook},
    {"A_IceGuyMissileExplode", A_IceGuyMissileExplode},
    {"A_IceGuyMissilePuff", A_IceGuyMissilePuff},
    {"A_IceSetTics", A_IceSetTics},
    {"A_KBolt", A_KBolt},
    {"A_KBoltRaise", A_KBoltRaise},
    {"A_KoraxBonePop", A_KoraxBonePop},
    {"A_KoraxChase", A_KoraxChase},
    {"A_KoraxCommand", A_KoraxCommand},
    {"A_KoraxDecide", A_KoraxDecide},
    {"A_KoraxMissile", A_KoraxMissile},
    {"A_KoraxStep", A_KoraxStep},
    {"A_KoraxStep2", A_KoraxStep2},
    {"A_KSpiritRoam", A_KSpiritRoam},
    {"A_LastZap", A_LastZap},
    {"A_LeafCheck", A_LeafCheck},
    {"A_LeafSpawn", A_LeafSpawn},
    {"A_LeafThrust", A_LeafThrust},
    {"A_Light0", A_Light0},
    {"A_LightningClip", A_LightningClip},
    {"A_LightningReady", A_LightningReady},
    {"A_LightningRemove", A_LightningRemove},
    {"A_LightningZap", A_LightningZap},
    {"A_Look", A_Look},
    {"A_Lower", A_Lower},
    {"A_MageAttack", A_MageAttack},
    {"A_MinotaurAtk1", A_MinotaurAtk1},
    {"A_MinotaurAtk2", A_MinotaurAtk2},
    {"A_MinotaurAtk3", A_MinotaurAtk3},
    {"A_MinotaurCharge", A_MinotaurCharge},
    {"A_MinotaurChase", A_MinotaurChase},
    {"A_MinotaurDecide", A_MinotaurDecide},
    {"A_MinotaurFade0", A_MinotaurFade0},
    {"A_MinotaurFade1", A_MinotaurFade1},
    {"A_MinotaurFade2", A_MinotaurFade2},
    {"A_MinotaurLook", A_MinotaurLook},
    {"A_MinotaurRoam", A_MinotaurRoam},
    {"A_MLightningAttack", A_MLightningAttack},
    {"A_MntrFloorFire", A_MntrFloorFire},
    {"A_MStaffAttack", A_MStaffAttack},
    {"A_MStaffPalette", A_MStaffPalette},
    {"A_MStaffTrack", A_MStaffTrack},
    {"A_MStaffWeave", A_MStaffWeave},
    {"A_MWandAttack", A_MWandAttack},
    {"A_NoBlocking", A_NoBlocking},
    {"A_NoGravity", A_NoGravity},
    {"A_Pain", A_Pain},
    {"A_PigAttack", A_PigAttack},
    {"A_PigChase", A_PigChase},
    {"A_PigLook", A_PigLook},
    {"A_PigPain", A_PigPain},
    {"A_PoisonBagCheck", A_PoisonBagCheck},
    {"A_PoisonBagDamage", A_PoisonBagDamage},
    {"A_PoisonBagInit", A_PoisonBagInit},
    {"A_PoisonShroom", A_PoisonShroom},
    {"A_PotteryCheck", A_PotteryCheck},
    {"A_PotteryChooseBit", A_PotteryChooseBit},
    {"A_PotteryExplode", A_PotteryExplode},
    {"A_Quake", A_Quake},
    {"A_QueueCorpse", A_QueueCorpse},
    {"A_Raise", A_Raise},
    {"A_ReFire", A_ReFire},
    {"A_RestoreArtifact", A_RestoreArtifact},
    {"A_RestoreSpecialThing1", A_RestoreSpecialThing1},
    {"A_RestoreSpecialThing2", A_RestoreSpecialThing2},
    {"A_Scream", A_Scream},
    {"A_SerpentBirthScream", A_SerpentBirthScream},
    {"A_SerpentChase", A_SerpentChase},
    {"A_SerpentCheckForAttack", A_SerpentCheckForAttack},
    {"A_SerpentChooseAttack", A_SerpentChooseAttack},
    {"A_SerpentDiveSound", A_SerpentDiveSound},
    {"A_SerpentHeadCheck", A_SerpentHeadCheck},
    {"A_SerpentHeadPop", A_SerpentHeadPop},
    {"A_SerpentHide", A_SerpentHide},
    {"A_SerpentHumpDecide", A_SerpentHumpDecide},
    {"A_SerpentLowerHump", A_SerpentLowerHump},
    {"A_SerpentMeleeAttack", A_SerpentMeleeAttack},
    {"A_SerpentMissileAttack", A_SerpentMissileAttack},
    {"A_SerpentRaiseHump", A_SerpentRaiseHump},
    {"A_SerpentSpawnGibs", A_SerpentSpawnGibs},
    {"A_SerpentUnHide", A_SerpentUnHide},
    {"A_SerpentWalk", A_SerpentWalk},
    {"A_SetAltShadow", A_SetAltShadow},
    {"A_SetReflective", A_SetReflective},
    {"A_SetShootable", A_SetShootable},
    {"A_ShedShard", A_ShedShard},
    {"A_SinkGib", A_SinkGib},
    {"A_SkullPop", A_SkullPop},
    {"A_SmBounce", A_SmBounce},
    {"A_SmokePuffExit", A_SmokePuffExit},
    {"A_SnoutAttack", A_SnoutAttack},
    {"A_SoAExplode", A_SoAExplode},
    {"A_SorcBallOrbit", A_SorcBallOrbit},
    {"A_SorcBallPop", A_SorcBallPop},
    {"A_SorcBossAttack", A_SorcBossAttack},
    {"A_SorcererBishopEntry", A_SorcererBishopEntry},
    {"A_SorcFX1Seek", A_SorcFX1Seek},
    {"A_SorcFX2Orbit", A_SorcFX2Orbit},
    {"A_SorcFX2Split", A_SorcFX2Split},
    {"A_SorcFX4Check", A_SorcFX4Check},
    {"A_SorcSpinBalls", A_SorcSpinBalls},
    {"A_SpawnBishop", A_SpawnBishop},
    {"A_SpawnFizzle", A_SpawnFizzle},
    {"A_SpeedBalls", A_SpeedBalls},
    {"A_SpeedFade", A_SpeedFade},
    {"A_Summon", A_Summon},
    {"A_TeloSpawnA", A_TeloSpawnA},
    {"A_TeloSpawnB", A_TeloSpawnB},
    {"A_TeloSpawnC", A_TeloSpawnC},
    {"A_TeloSpawnD", A_TeloSpawnD},
    {"A_ThrustBlock", A_ThrustBlock},
    {"A_ThrustImpale", A_ThrustImpale},
    {"A_ThrustInitDn", A_ThrustInitDn},
    {"A_ThrustInitUp", A_ThrustInitUp},
    {"A_ThrustLower", A_ThrustLower},
    {"A_ThrustRaise", A_ThrustRaise},
    {"A_TreeDeath", A_TreeDeath},
    {"A_UnHideThing", A_UnHideThing},
    {"A_UnSetInvulnerable", A_UnSetInvulnerable},
    {"A_UnSetReflective", A_UnSetReflective},
    {"A_UnSetShootable", A_UnSetShootable},
    {"A_WeaponReady", A_WeaponReady},
    {"A_WraithChase", A_WraithChase},
    {"A_WraithFX2", A_WraithFX2},
    {"A_WraithFX3", A_WraithFX3},
    {"A_WraithInit", A_WraithInit},
    {"A_WraithLook", A_WraithLook},
    {"A_WraithMelee", A_WraithMelee},
    {"A_WraithMissile", A_WraithMissile},
    {"A_WraithRaise", A_WraithRaise},
    {"A_WraithRaiseInit", A_WraithRaiseInit},
    {"A_ZapMimic", A_ZapMimic},
    {0, 0}
};

// PRIVATE DATA DEFINITIONS ------------------------------------------------

// CODE --------------------------------------------------------------------
