/**\file
 *\section License
 * License: GPL + jHeretic/jHexen Exception
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright © 2003-2007 Jaakko Keränen <skyjake@dengine.net>
 *\author Copyright © 2005-2006 Daniel Swanson <danij@dengine.net>
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

// Sprite, state, mobjtype and text identifiers
// Generated with DED Manager 1.1

#ifndef __INFO_CONSTANTS_H__
#define __INFO_CONSTANTS_H__

#include "xgclass.h"

// Sprites.
typedef enum {
    SPR_IMPX,                       // 000
    SPR_ACLO,                       // 001
    SPR_PTN1,                       // 002
    SPR_SHLD,                       // 003
    SPR_SHD2,                       // 004
    SPR_BAGH,                       // 005
    SPR_SPMP,                       // 006
    SPR_INVS,                       // 007
    SPR_PTN2,                       // 008
    SPR_SOAR,                       // 009
    SPR_INVU,                       // 010
    SPR_PWBK,                       // 011
    SPR_EGGC,                       // 012
    SPR_EGGM,                       // 013
    SPR_FX01,                       // 014
    SPR_SPHL,                       // 015
    SPR_TRCH,                       // 016
    SPR_FBMB,                       // 017
    SPR_XPL1,                       // 018
    SPR_ATLP,                       // 019
    SPR_PPOD,                       // 020
    SPR_AMG1,                       // 021
    SPR_SPSH,                       // 022
    SPR_LVAS,                       // 023
    SPR_SLDG,                       // 024
    SPR_SKH1,                       // 025
    SPR_SKH2,                       // 026
    SPR_SKH3,                       // 027
    SPR_SKH4,                       // 028
    SPR_CHDL,                       // 029
    SPR_SRTC,                       // 030
    SPR_SMPL,                       // 031
    SPR_STGS,                       // 032
    SPR_STGL,                       // 033
    SPR_STCS,                       // 034
    SPR_STCL,                       // 035
    SPR_KFR1,                       // 036
    SPR_BARL,                       // 037
    SPR_BRPL,                       // 038
    SPR_MOS1,                       // 039
    SPR_MOS2,                       // 040
    SPR_WTRH,                       // 041
    SPR_HCOR,                       // 042
    SPR_KGZ1,                       // 043
    SPR_KGZB,                       // 044
    SPR_KGZG,                       // 045
    SPR_KGZY,                       // 046
    SPR_VLCO,                       // 047
    SPR_VFBL,                       // 048
    SPR_VTFB,                       // 049
    SPR_SFFI,                       // 050
    SPR_TGLT,                       // 051
    SPR_TELE,                       // 052
    SPR_STFF,                       // 053
    SPR_PUF3,                       // 054
    SPR_PUF4,                       // 055
    SPR_BEAK,                       // 056
    SPR_WGNT,                       // 057
    SPR_GAUN,                       // 058
    SPR_PUF1,                       // 059
    SPR_WBLS,                       // 060
    SPR_BLSR,                       // 061
    SPR_FX18,                       // 062
    SPR_FX17,                       // 063
    SPR_WMCE,                       // 064
    SPR_MACE,                       // 065
    SPR_FX02,                       // 066
    SPR_WSKL,                       // 067
    SPR_HROD,                       // 068
    SPR_FX00,                       // 069
    SPR_FX20,                       // 070
    SPR_FX21,                       // 071
    SPR_FX22,                       // 072
    SPR_FX23,                       // 073
    SPR_GWND,                       // 074
    SPR_PUF2,                       // 075
    SPR_WPHX,                       // 076
    SPR_PHNX,                       // 077
    SPR_FX04,                       // 078
    SPR_FX08,                       // 079
    SPR_FX09,                       // 080
    SPR_WBOW,                       // 081
    SPR_CRBW,                       // 082
    SPR_FX03,                       // 083
    SPR_BLOD,                       // 084
    SPR_PLAY,                       // 085
    SPR_FDTH,                       // 086
    SPR_BSKL,                       // 087
    SPR_CHKN,                       // 088
    SPR_MUMM,                       // 089
    SPR_FX15,                       // 090
    SPR_BEAS,                       // 091
    SPR_FRB1,                       // 092
    SPR_SNKE,                       // 093
    SPR_SNFX,                       // 094
    SPR_HEAD,                       // 095
    SPR_FX05,                       // 096
    SPR_FX06,                       // 097
    SPR_FX07,                       // 098
    SPR_CLNK,                       // 099
    SPR_WZRD,                       // 100
    SPR_FX11,                       // 101
    SPR_FX10,                       // 102
    SPR_KNIG,                       // 103
    SPR_SPAX,                       // 104
    SPR_RAXE,                       // 105
    SPR_SRCR,                       // 106
    SPR_FX14,                       // 107
    SPR_SOR2,                       // 108
    SPR_SDTH,                       // 109
    SPR_FX16,                       // 110
    SPR_MNTR,                       // 111
    SPR_FX12,                       // 112
    SPR_FX13,                       // 113
    SPR_AKYY,                       // 114
    SPR_BKYY,                       // 115
    SPR_CKYY,                       // 116
    SPR_AMG2,                       // 117
    SPR_AMM1,                       // 118
    SPR_AMM2,                       // 119
    SPR_AMC1,                       // 120
    SPR_AMC2,                       // 121
    SPR_AMS1,                       // 122
    SPR_AMS2,                       // 123
    SPR_AMP1,                       // 124
    SPR_AMP2,                       // 125
    SPR_AMB1,                       // 126
    SPR_AMB2,                       // 127
    NUMSPRITES                       // 128
} spritetype_e;

// States.
typedef enum {
    S_NULL,                           // 000
    S_FREETARGMOBJ,                   // 001
    S_ITEM_PTN1_1,                   // 002
    S_ITEM_PTN1_2,                   // 003
    S_ITEM_PTN1_3,                   // 004
    S_ITEM_SHLD1,                   // 005
    S_ITEM_SHD2_1,                   // 006
    S_ITEM_BAGH1,                   // 007
    S_ITEM_SPMP1,                   // 008
    S_HIDESPECIAL1,                   // 009
    S_HIDESPECIAL2,                   // 010
    S_HIDESPECIAL3,                   // 011
    S_HIDESPECIAL4,                   // 012
    S_HIDESPECIAL5,                   // 013
    S_HIDESPECIAL6,                   // 014
    S_HIDESPECIAL7,                   // 015
    S_HIDESPECIAL8,                   // 016
    S_HIDESPECIAL9,                   // 017
    S_HIDESPECIAL10,               // 018
    S_HIDESPECIAL11,               // 019
    S_DORMANTARTI1,                   // 020
    S_DORMANTARTI2,                   // 021
    S_DORMANTARTI3,                   // 022
    S_DORMANTARTI4,                   // 023
    S_DORMANTARTI5,                   // 024
    S_DORMANTARTI6,                   // 025
    S_DORMANTARTI7,                   // 026
    S_DORMANTARTI8,                   // 027
    S_DORMANTARTI9,                   // 028
    S_DORMANTARTI10,               // 029
    S_DORMANTARTI11,               // 030
    S_DORMANTARTI12,               // 031
    S_DORMANTARTI13,               // 032
    S_DORMANTARTI14,               // 033
    S_DORMANTARTI15,               // 034
    S_DORMANTARTI16,               // 035
    S_DORMANTARTI17,               // 036
    S_DORMANTARTI18,               // 037
    S_DORMANTARTI19,               // 038
    S_DORMANTARTI20,               // 039
    S_DORMANTARTI21,               // 040
    S_DEADARTI1,                   // 041
    S_DEADARTI2,                   // 042
    S_DEADARTI3,                   // 043
    S_DEADARTI4,                   // 044
    S_DEADARTI5,                   // 045
    S_DEADARTI6,                   // 046
    S_DEADARTI7,                   // 047
    S_DEADARTI8,                   // 048
    S_DEADARTI9,                   // 049
    S_DEADARTI10,                   // 050
    S_ARTI_INVS1,                   // 051
    S_ARTI_PTN2_1,                   // 052
    S_ARTI_PTN2_2,                   // 053
    S_ARTI_PTN2_3,                   // 054
    S_ARTI_SOAR1,                   // 055
    S_ARTI_SOAR2,                   // 056
    S_ARTI_SOAR3,                   // 057
    S_ARTI_SOAR4,                   // 058
    S_ARTI_INVU1,                   // 059
    S_ARTI_INVU2,                   // 060
    S_ARTI_INVU3,                   // 061
    S_ARTI_INVU4,                   // 062
    S_ARTI_PWBK1,                   // 063
    S_ARTI_EGGC1,                   // 064
    S_ARTI_EGGC2,                   // 065
    S_ARTI_EGGC3,                   // 066
    S_ARTI_EGGC4,                   // 067
    S_EGGFX1,                       // 068
    S_EGGFX2,                       // 069
    S_EGGFX3,                       // 070
    S_EGGFX4,                       // 071
    S_EGGFX5,                       // 072
    S_EGGFXI1_1,                   // 073
    S_EGGFXI1_2,                   // 074
    S_EGGFXI1_3,                   // 075
    S_EGGFXI1_4,                   // 076
    S_ARTI_SPHL1,                   // 077
    S_ARTI_TRCH1,                   // 078
    S_ARTI_TRCH2,                   // 079
    S_ARTI_TRCH3,                   // 080
    S_ARTI_FBMB1,                   // 081
    S_FIREBOMB1,                   // 082
    S_FIREBOMB2,                   // 083
    S_FIREBOMB3,                   // 084
    S_FIREBOMB4,                   // 085
    S_FIREBOMB5,                   // 086
    S_FIREBOMB6,                   // 087
    S_FIREBOMB7,                   // 088
    S_FIREBOMB8,                   // 089
    S_FIREBOMB9,                   // 090
    S_FIREBOMB10,                   // 091
    S_FIREBOMB11,                   // 092
    S_ARTI_ATLP1,                   // 093
    S_ARTI_ATLP2,                   // 094
    S_ARTI_ATLP3,                   // 095
    S_ARTI_ATLP4,                   // 096
    S_POD_WAIT1,                   // 097
    S_POD_PAIN1,                   // 098
    S_POD_DIE1,                       // 099
    S_POD_DIE2,                       // 100
    S_POD_DIE3,                       // 101
    S_POD_DIE4,                       // 102
    S_POD_GROW1,                   // 103
    S_POD_GROW2,                   // 104
    S_POD_GROW3,                   // 105
    S_POD_GROW4,                   // 106
    S_POD_GROW5,                   // 107
    S_POD_GROW6,                   // 108
    S_POD_GROW7,                   // 109
    S_POD_GROW8,                   // 110
    S_PODGOO1,                       // 111
    S_PODGOO2,                       // 112
    S_PODGOOX,                       // 113
    S_PODGENERATOR,                   // 114
    S_SPLASH1,                       // 115
    S_SPLASH2,                       // 116
    S_SPLASH3,                       // 117
    S_SPLASH4,                       // 118
    S_SPLASHX,                       // 119
    S_SPLASHBASE1,                   // 120
    S_SPLASHBASE2,                   // 121
    S_SPLASHBASE3,                   // 122
    S_SPLASHBASE4,                   // 123
    S_SPLASHBASE5,                   // 124
    S_SPLASHBASE6,                   // 125
    S_SPLASHBASE7,                   // 126
    S_LAVASPLASH1,                   // 127
    S_LAVASPLASH2,                   // 128
    S_LAVASPLASH3,                   // 129
    S_LAVASPLASH4,                   // 130
    S_LAVASPLASH5,                   // 131
    S_LAVASPLASH6,                   // 132
    S_LAVASMOKE1,                   // 133
    S_LAVASMOKE2,                   // 134
    S_LAVASMOKE3,                   // 135
    S_LAVASMOKE4,                   // 136
    S_LAVASMOKE5,                   // 137
    S_SLUDGECHUNK1,                   // 138
    S_SLUDGECHUNK2,                   // 139
    S_SLUDGECHUNK3,                   // 140
    S_SLUDGECHUNK4,                   // 141
    S_SLUDGECHUNKX,                   // 142
    S_SLUDGESPLASH1,               // 143
    S_SLUDGESPLASH2,               // 144
    S_SLUDGESPLASH3,               // 145
    S_SLUDGESPLASH4,               // 146
    S_SKULLHANG70_1,               // 147
    S_SKULLHANG60_1,               // 148
    S_SKULLHANG45_1,               // 149
    S_SKULLHANG35_1,               // 150
    S_CHANDELIER1,                   // 151
    S_CHANDELIER2,                   // 152
    S_CHANDELIER3,                   // 153
    S_SERPTORCH1,                   // 154
    S_SERPTORCH2,                   // 155
    S_SERPTORCH3,                   // 156
    S_SMALLPILLAR,                   // 157
    S_STALAGMITESMALL,               // 158
    S_STALAGMITELARGE,               // 159
    S_STALACTITESMALL,               // 160
    S_STALACTITELARGE,               // 161
    S_FIREBRAZIER1,                   // 162
    S_FIREBRAZIER2,                   // 163
    S_FIREBRAZIER3,                   // 164
    S_FIREBRAZIER4,                   // 165
    S_FIREBRAZIER5,                   // 166
    S_FIREBRAZIER6,                   // 167
    S_FIREBRAZIER7,                   // 168
    S_FIREBRAZIER8,                   // 169
    S_BARREL,                       // 170
    S_BRPILLAR,                       // 171
    S_MOSS1,                       // 172
    S_MOSS2,                       // 173
    S_WALLTORCH1,                   // 174
    S_WALLTORCH2,                   // 175
    S_WALLTORCH3,                   // 176
    S_HANGINGCORPSE,               // 177
    S_KEYGIZMO1,                   // 178
    S_KEYGIZMO2,                   // 179
    S_KEYGIZMO3,                   // 180
    S_KGZ_START,                   // 181
    S_KGZ_BLUEFLOAT1,               // 182
    S_KGZ_GREENFLOAT1,               // 183
    S_KGZ_YELLOWFLOAT1,               // 184
    S_VOLCANO1,                       // 185
    S_VOLCANO2,                       // 186
    S_VOLCANO3,                       // 187
    S_VOLCANO4,                       // 188
    S_VOLCANO5,                       // 189
    S_VOLCANO6,                       // 190
    S_VOLCANO7,                       // 191
    S_VOLCANO8,                       // 192
    S_VOLCANO9,                       // 193
    S_VOLCANOBALL1,                   // 194
    S_VOLCANOBALL2,                   // 195
    S_VOLCANOBALLX1,               // 196
    S_VOLCANOBALLX2,               // 197
    S_VOLCANOBALLX3,               // 198
    S_VOLCANOBALLX4,               // 199
    S_VOLCANOBALLX5,               // 200
    S_VOLCANOBALLX6,               // 201
    S_VOLCANOTBALL1,               // 202
    S_VOLCANOTBALL2,               // 203
    S_VOLCANOTBALLX1,               // 204
    S_VOLCANOTBALLX2,               // 205
    S_VOLCANOTBALLX3,               // 206
    S_VOLCANOTBALLX4,               // 207
    S_VOLCANOTBALLX5,               // 208
    S_VOLCANOTBALLX6,               // 209
    S_VOLCANOTBALLX7,               // 210
    S_TELEGLITGEN1,                   // 211
    S_TELEGLITGEN2,                   // 212
    S_TELEGLITTER1_1,               // 213
    S_TELEGLITTER1_2,               // 214
    S_TELEGLITTER1_3,               // 215
    S_TELEGLITTER1_4,               // 216
    S_TELEGLITTER1_5,               // 217
    S_TELEGLITTER2_1,               // 218
    S_TELEGLITTER2_2,               // 219
    S_TELEGLITTER2_3,               // 220
    S_TELEGLITTER2_4,               // 221
    S_TELEGLITTER2_5,               // 222
    S_TFOG1,                       // 223
    S_TFOG2,                       // 224
    S_TFOG3,                       // 225
    S_TFOG4,                       // 226
    S_TFOG5,                       // 227
    S_TFOG6,                       // 228
    S_TFOG7,                       // 229
    S_TFOG8,                       // 230
    S_TFOG9,                       // 231
    S_TFOG10,                       // 232
    S_TFOG11,                       // 233
    S_TFOG12,                       // 234
    S_TFOG13,                       // 235
    S_LIGHTDONE,                   // 236
    S_STAFFREADY,                   // 237
    S_STAFFDOWN,                   // 238
    S_STAFFUP,                       // 239
    S_STAFFREADY2_1,               // 240
    S_STAFFREADY2_2,               // 241
    S_STAFFREADY2_3,               // 242
    S_STAFFDOWN2,                   // 243
    S_STAFFUP2,                       // 244
    S_STAFFATK1_1,                   // 245
    S_STAFFATK1_2,                   // 246
    S_STAFFATK1_3,                   // 247
    S_STAFFATK2_1,                   // 248
    S_STAFFATK2_2,                   // 249
    S_STAFFATK2_3,                   // 250
    S_STAFFPUFF1,                   // 251
    S_STAFFPUFF2,                   // 252
    S_STAFFPUFF3,                   // 253
    S_STAFFPUFF4,                   // 254
    S_STAFFPUFF2_1,                   // 255
    S_STAFFPUFF2_2,                   // 256
    S_STAFFPUFF2_3,                   // 257
    S_STAFFPUFF2_4,                   // 258
    S_STAFFPUFF2_5,                   // 259
    S_STAFFPUFF2_6,                   // 260
    S_BEAKREADY,                   // 261
    S_BEAKDOWN,                       // 262
    S_BEAKUP,                       // 263
    S_BEAKATK1_1,                   // 264
    S_BEAKATK2_1,                   // 265
    S_WGNT,                           // 266
    S_GAUNTLETREADY,               // 267
    S_GAUNTLETDOWN,                   // 268
    S_GAUNTLETUP,                   // 269
    S_GAUNTLETREADY2_1,               // 270
    S_GAUNTLETREADY2_2,               // 271
    S_GAUNTLETREADY2_3,               // 272
    S_GAUNTLETDOWN2,               // 273
    S_GAUNTLETUP2,                   // 274
    S_GAUNTLETATK1_1,               // 275
    S_GAUNTLETATK1_2,               // 276
    S_GAUNTLETATK1_3,               // 277
    S_GAUNTLETATK1_4,               // 278
    S_GAUNTLETATK1_5,               // 279
    S_GAUNTLETATK1_6,               // 280
    S_GAUNTLETATK1_7,               // 281
    S_GAUNTLETATK2_1,               // 282
    S_GAUNTLETATK2_2,               // 283
    S_GAUNTLETATK2_3,               // 284
    S_GAUNTLETATK2_4,               // 285
    S_GAUNTLETATK2_5,               // 286
    S_GAUNTLETATK2_6,               // 287
    S_GAUNTLETATK2_7,               // 288
    S_GAUNTLETPUFF1_1,               // 289
    S_GAUNTLETPUFF1_2,               // 290
    S_GAUNTLETPUFF1_3,               // 291
    S_GAUNTLETPUFF1_4,               // 292
    S_GAUNTLETPUFF2_1,               // 293
    S_GAUNTLETPUFF2_2,               // 294
    S_GAUNTLETPUFF2_3,               // 295
    S_GAUNTLETPUFF2_4,               // 296
    S_BLSR,                           // 297
    S_BLASTERREADY,                   // 298
    S_BLASTERDOWN,                   // 299
    S_BLASTERUP,                   // 300
    S_BLASTERATK1_1,               // 301
    S_BLASTERATK1_2,               // 302
    S_BLASTERATK1_3,               // 303
    S_BLASTERATK1_4,               // 304
    S_BLASTERATK1_5,               // 305
    S_BLASTERATK1_6,               // 306
    S_BLASTERATK2_1,               // 307
    S_BLASTERATK2_2,               // 308
    S_BLASTERATK2_3,               // 309
    S_BLASTERATK2_4,               // 310
    S_BLASTERATK2_5,               // 311
    S_BLASTERATK2_6,               // 312
    S_BLASTERFX1_1,                   // 313
    S_BLASTERFXI1_1,               // 314
    S_BLASTERFXI1_2,               // 315
    S_BLASTERFXI1_3,               // 316
    S_BLASTERFXI1_4,               // 317
    S_BLASTERFXI1_5,               // 318
    S_BLASTERFXI1_6,               // 319
    S_BLASTERFXI1_7,               // 320
    S_BLASTERSMOKE1,               // 321
    S_BLASTERSMOKE2,               // 322
    S_BLASTERSMOKE3,               // 323
    S_BLASTERSMOKE4,               // 324
    S_BLASTERSMOKE5,               // 325
    S_RIPPER1,                       // 326
    S_RIPPER2,                       // 327
    S_RIPPERX1,                       // 328
    S_RIPPERX2,                       // 329
    S_RIPPERX3,                       // 330
    S_RIPPERX4,                       // 331
    S_RIPPERX5,                       // 332
    S_BLASTERPUFF1_1,               // 333
    S_BLASTERPUFF1_2,               // 334
    S_BLASTERPUFF1_3,               // 335
    S_BLASTERPUFF1_4,               // 336
    S_BLASTERPUFF1_5,               // 337
    S_BLASTERPUFF2_1,               // 338
    S_BLASTERPUFF2_2,               // 339
    S_BLASTERPUFF2_3,               // 340
    S_BLASTERPUFF2_4,               // 341
    S_BLASTERPUFF2_5,               // 342
    S_BLASTERPUFF2_6,               // 343
    S_BLASTERPUFF2_7,               // 344
    S_WMCE,                           // 345
    S_MACEREADY,                   // 346
    S_MACEDOWN,                       // 347
    S_MACEUP,                       // 348
    S_MACEATK1_1,                   // 349
    S_MACEATK1_2,                   // 350
    S_MACEATK1_3,                   // 351
    S_MACEATK1_4,                   // 352
    S_MACEATK1_5,                   // 353
    S_MACEATK1_6,                   // 354
    S_MACEATK1_7,                   // 355
    S_MACEATK1_8,                   // 356
    S_MACEATK1_9,                   // 357
    S_MACEATK1_10,                   // 358
    S_MACEATK2_1,                   // 359
    S_MACEATK2_2,                   // 360
    S_MACEATK2_3,                   // 361
    S_MACEATK2_4,                   // 362
    S_MACEFX1_1,                   // 363
    S_MACEFX1_2,                   // 364
    S_MACEFXI1_1,                   // 365
    S_MACEFXI1_2,                   // 366
    S_MACEFXI1_3,                   // 367
    S_MACEFXI1_4,                   // 368
    S_MACEFXI1_5,                   // 369
    S_MACEFX2_1,                   // 370
    S_MACEFX2_2,                   // 371
    S_MACEFXI2_1,                   // 372
    S_MACEFX3_1,                   // 373
    S_MACEFX3_2,                   // 374
    S_MACEFX4_1,                   // 375
    S_MACEFXI4_1,                   // 376
    S_WSKL,                           // 377
    S_HORNRODREADY,                   // 378
    S_HORNRODDOWN,                   // 379
    S_HORNRODUP,                   // 380
    S_HORNRODATK1_1,               // 381
    S_HORNRODATK1_2,               // 382
    S_HORNRODATK1_3,               // 383
    S_HORNRODATK2_1,               // 384
    S_HORNRODATK2_2,               // 385
    S_HORNRODATK2_3,               // 386
    S_HORNRODATK2_4,               // 387
    S_HORNRODATK2_5,               // 388
    S_HORNRODATK2_6,               // 389
    S_HORNRODATK2_7,               // 390
    S_HORNRODATK2_8,               // 391
    S_HORNRODATK2_9,               // 392
    S_HRODFX1_1,                   // 393
    S_HRODFX1_2,                   // 394
    S_HRODFXI1_1,                   // 395
    S_HRODFXI1_2,                   // 396
    S_HRODFXI1_3,                   // 397
    S_HRODFXI1_4,                   // 398
    S_HRODFXI1_5,                   // 399
    S_HRODFXI1_6,                   // 400
    S_HRODFX2_1,                   // 401
    S_HRODFX2_2,                   // 402
    S_HRODFX2_3,                   // 403
    S_HRODFX2_4,                   // 404
    S_HRODFXI2_1,                   // 405
    S_HRODFXI2_2,                   // 406
    S_HRODFXI2_3,                   // 407
    S_HRODFXI2_4,                   // 408
    S_HRODFXI2_5,                   // 409
    S_HRODFXI2_6,                   // 410
    S_HRODFXI2_7,                   // 411
    S_HRODFXI2_8,                   // 412
    S_RAINPLR1_1,                   // 413
    S_RAINPLR2_1,                   // 414
    S_RAINPLR3_1,                   // 415
    S_RAINPLR4_1,                   // 416
    S_RAINPLR1X_1,                   // 417
    S_RAINPLR1X_2,                   // 418
    S_RAINPLR1X_3,                   // 419
    S_RAINPLR1X_4,                   // 420
    S_RAINPLR1X_5,                   // 421
    S_RAINPLR2X_1,                   // 422
    S_RAINPLR2X_2,                   // 423
    S_RAINPLR2X_3,                   // 424
    S_RAINPLR2X_4,                   // 425
    S_RAINPLR2X_5,                   // 426
    S_RAINPLR3X_1,                   // 427
    S_RAINPLR3X_2,                   // 428
    S_RAINPLR3X_3,                   // 429
    S_RAINPLR3X_4,                   // 430
    S_RAINPLR3X_5,                   // 431
    S_RAINPLR4X_1,                   // 432
    S_RAINPLR4X_2,                   // 433
    S_RAINPLR4X_3,                   // 434
    S_RAINPLR4X_4,                   // 435
    S_RAINPLR4X_5,                   // 436
    S_RAINAIRXPLR1_1,               // 437
    S_RAINAIRXPLR2_1,               // 438
    S_RAINAIRXPLR3_1,               // 439
    S_RAINAIRXPLR4_1,               // 440
    S_RAINAIRXPLR1_2,               // 441
    S_RAINAIRXPLR2_2,               // 442
    S_RAINAIRXPLR3_2,               // 443
    S_RAINAIRXPLR4_2,               // 444
    S_RAINAIRXPLR1_3,               // 445
    S_RAINAIRXPLR2_3,               // 446
    S_RAINAIRXPLR3_3,               // 447
    S_RAINAIRXPLR4_3,               // 448
    S_GOLDWANDREADY,               // 449
    S_GOLDWANDDOWN,                   // 450
    S_GOLDWANDUP,                   // 451
    S_GOLDWANDATK1_1,               // 452
    S_GOLDWANDATK1_2,               // 453
    S_GOLDWANDATK1_3,               // 454
    S_GOLDWANDATK1_4,               // 455
    S_GOLDWANDATK2_1,               // 456
    S_GOLDWANDATK2_2,               // 457
    S_GOLDWANDATK2_3,               // 458
    S_GOLDWANDATK2_4,               // 459
    S_GWANDFX1_1,                   // 460
    S_GWANDFX1_2,                   // 461
    S_GWANDFXI1_1,                   // 462
    S_GWANDFXI1_2,                   // 463
    S_GWANDFXI1_3,                   // 464
    S_GWANDFXI1_4,                   // 465
    S_GWANDFX2_1,                   // 466
    S_GWANDFX2_2,                   // 467
    S_GWANDPUFF1_1,                   // 468
    S_GWANDPUFF1_2,                   // 469
    S_GWANDPUFF1_3,                   // 470
    S_GWANDPUFF1_4,                   // 471
    S_GWANDPUFF1_5,                   // 472
    S_WPHX,                           // 473
    S_PHOENIXREADY,                   // 474
    S_PHOENIXDOWN,                   // 475
    S_PHOENIXUP,                   // 476
    S_PHOENIXATK1_1,               // 477
    S_PHOENIXATK1_2,               // 478
    S_PHOENIXATK1_3,               // 479
    S_PHOENIXATK1_4,               // 480
    S_PHOENIXATK1_5,               // 481
    S_PHOENIXATK2_1,               // 482
    S_PHOENIXATK2_2,               // 483
    S_PHOENIXATK2_3,               // 484
    S_PHOENIXATK2_4,               // 485
    S_PHOENIXFX1_1,                   // 486
    S_PHOENIXFXI1_1,               // 487
    S_PHOENIXFXI1_2,               // 488
    S_PHOENIXFXI1_3,               // 489
    S_PHOENIXFXI1_4,               // 490
    S_PHOENIXFXI1_5,               // 491
    S_PHOENIXFXI1_6,               // 492
    S_PHOENIXFXI1_7,               // 493
    S_PHOENIXFXI1_8,               // 494
    S_PHOENIXPUFF1,                   // 495
    S_PHOENIXPUFF2,                   // 496
    S_PHOENIXPUFF3,                   // 497
    S_PHOENIXPUFF4,                   // 498
    S_PHOENIXPUFF5,                   // 499
    S_PHOENIXFX2_1,                   // 500
    S_PHOENIXFX2_2,                   // 501
    S_PHOENIXFX2_3,                   // 502
    S_PHOENIXFX2_4,                   // 503
    S_PHOENIXFX2_5,                   // 504
    S_PHOENIXFX2_6,                   // 505
    S_PHOENIXFX2_7,                   // 506
    S_PHOENIXFX2_8,                   // 507
    S_PHOENIXFX2_9,                   // 508
    S_PHOENIXFX2_10,               // 509
    S_PHOENIXFXI2_1,               // 510
    S_PHOENIXFXI2_2,               // 511
    S_PHOENIXFXI2_3,               // 512
    S_PHOENIXFXI2_4,               // 513
    S_PHOENIXFXI2_5,               // 514
    S_WBOW,                           // 515
    S_CRBOW1,                       // 516
    S_CRBOW2,                       // 517
    S_CRBOW3,                       // 518
    S_CRBOW4,                       // 519
    S_CRBOW5,                       // 520
    S_CRBOW6,                       // 521
    S_CRBOW7,                       // 522
    S_CRBOW8,                       // 523
    S_CRBOW9,                       // 524
    S_CRBOW10,                       // 525
    S_CRBOW11,                       // 526
    S_CRBOW12,                       // 527
    S_CRBOW13,                       // 528
    S_CRBOW14,                       // 529
    S_CRBOW15,                       // 530
    S_CRBOW16,                       // 531
    S_CRBOW17,                       // 532
    S_CRBOW18,                       // 533
    S_CRBOWDOWN,                   // 534
    S_CRBOWUP,                       // 535
    S_CRBOWATK1_1,                   // 536
    S_CRBOWATK1_2,                   // 537
    S_CRBOWATK1_3,                   // 538
    S_CRBOWATK1_4,                   // 539
    S_CRBOWATK1_5,                   // 540
    S_CRBOWATK1_6,                   // 541
    S_CRBOWATK1_7,                   // 542
    S_CRBOWATK1_8,                   // 543
    S_CRBOWATK2_1,                   // 544
    S_CRBOWATK2_2,                   // 545
    S_CRBOWATK2_3,                   // 546
    S_CRBOWATK2_4,                   // 547
    S_CRBOWATK2_5,                   // 548
    S_CRBOWATK2_6,                   // 549
    S_CRBOWATK2_7,                   // 550
    S_CRBOWATK2_8,                   // 551
    S_CRBOWFX1,                       // 552
    S_CRBOWFXI1_1,                   // 553
    S_CRBOWFXI1_2,                   // 554
    S_CRBOWFXI1_3,                   // 555
    S_CRBOWFX2,                       // 556
    S_CRBOWFX3,                       // 557
    S_CRBOWFXI3_1,                   // 558
    S_CRBOWFXI3_2,                   // 559
    S_CRBOWFXI3_3,                   // 560
    S_CRBOWFX4_1,                   // 561
    S_CRBOWFX4_2,                   // 562
    S_BLOOD1,                       // 563
    S_BLOOD2,                       // 564
    S_BLOOD3,                       // 565
    S_BLOODSPLATTER1,               // 566
    S_BLOODSPLATTER2,               // 567
    S_BLOODSPLATTER3,               // 568
    S_BLOODSPLATTERX,               // 569
    S_PLAY,                           // 570
    S_PLAY_RUN1,                   // 571
    S_PLAY_RUN2,                   // 572
    S_PLAY_RUN3,                   // 573
    S_PLAY_RUN4,                   // 574
    S_PLAY_ATK1,                   // 575
    S_PLAY_ATK2,                   // 576
    S_PLAY_PAIN,                   // 577
    S_PLAY_PAIN2,                   // 578
    S_PLAY_DIE1,                   // 579
    S_PLAY_DIE2,                   // 580
    S_PLAY_DIE3,                   // 581
    S_PLAY_DIE4,                   // 582
    S_PLAY_DIE5,                   // 583
    S_PLAY_DIE6,                   // 584
    S_PLAY_DIE7,                   // 585
    S_PLAY_DIE8,                   // 586
    S_PLAY_DIE9,                   // 587
    S_PLAY_XDIE1,                   // 588
    S_PLAY_XDIE2,                   // 589
    S_PLAY_XDIE3,                   // 590
    S_PLAY_XDIE4,                   // 591
    S_PLAY_XDIE5,                   // 592
    S_PLAY_XDIE6,                   // 593
    S_PLAY_XDIE7,                   // 594
    S_PLAY_XDIE8,                   // 595
    S_PLAY_XDIE9,                   // 596
    S_PLAY_FDTH1,                   // 597
    S_PLAY_FDTH2,                   // 598
    S_PLAY_FDTH3,                   // 599
    S_PLAY_FDTH4,                   // 600
    S_PLAY_FDTH5,                   // 601
    S_PLAY_FDTH6,                   // 602
    S_PLAY_FDTH7,                   // 603
    S_PLAY_FDTH8,                   // 604
    S_PLAY_FDTH9,                   // 605
    S_PLAY_FDTH10,                   // 606
    S_PLAY_FDTH11,                   // 607
    S_PLAY_FDTH12,                   // 608
    S_PLAY_FDTH13,                   // 609
    S_PLAY_FDTH14,                   // 610
    S_PLAY_FDTH15,                   // 611
    S_PLAY_FDTH16,                   // 612
    S_PLAY_FDTH17,                   // 613
    S_PLAY_FDTH18,                   // 614
    S_PLAY_FDTH19,                   // 615
    S_PLAY_FDTH20,                   // 616
    S_BLOODYSKULL1,                   // 617
    S_BLOODYSKULL2,                   // 618
    S_BLOODYSKULL3,                   // 619
    S_BLOODYSKULL4,                   // 620
    S_BLOODYSKULL5,                   // 621
    S_BLOODYSKULLX1,               // 622
    S_BLOODYSKULLX2,               // 623
    S_CHICPLAY,                       // 624
    S_CHICPLAY_RUN1,               // 625
    S_CHICPLAY_RUN2,               // 626
    S_CHICPLAY_RUN3,               // 627
    S_CHICPLAY_RUN4,               // 628
    S_CHICPLAY_ATK1,               // 629
    S_CHICPLAY_PAIN,               // 630
    S_CHICPLAY_PAIN2,               // 631
    S_CHICKEN_LOOK1,               // 632
    S_CHICKEN_LOOK2,               // 633
    S_CHICKEN_WALK1,               // 634
    S_CHICKEN_WALK2,               // 635
    S_CHICKEN_PAIN1,               // 636
    S_CHICKEN_PAIN2,               // 637
    S_CHICKEN_ATK1,                   // 638
    S_CHICKEN_ATK2,                   // 639
    S_CHICKEN_DIE1,                   // 640
    S_CHICKEN_DIE2,                   // 641
    S_CHICKEN_DIE3,                   // 642
    S_CHICKEN_DIE4,                   // 643
    S_CHICKEN_DIE5,                   // 644
    S_CHICKEN_DIE6,                   // 645
    S_CHICKEN_DIE7,                   // 646
    S_CHICKEN_DIE8,                   // 647
    S_FEATHER1,                       // 648
    S_FEATHER2,                       // 649
    S_FEATHER3,                       // 650
    S_FEATHER4,                       // 651
    S_FEATHER5,                       // 652
    S_FEATHER6,                       // 653
    S_FEATHER7,                       // 654
    S_FEATHER8,                       // 655
    S_FEATHERX,                       // 656
    S_MUMMY_LOOK1,                   // 657
    S_MUMMY_LOOK2,                   // 658
    S_MUMMY_WALK1,                   // 659
    S_MUMMY_WALK2,                   // 660
    S_MUMMY_WALK3,                   // 661
    S_MUMMY_WALK4,                   // 662
    S_MUMMY_ATK1,                   // 663
    S_MUMMY_ATK2,                   // 664
    S_MUMMY_ATK3,                   // 665
    S_MUMMYL_ATK1,                   // 666
    S_MUMMYL_ATK2,                   // 667
    S_MUMMYL_ATK3,                   // 668
    S_MUMMYL_ATK4,                   // 669
    S_MUMMYL_ATK5,                   // 670
    S_MUMMYL_ATK6,                   // 671
    S_MUMMY_PAIN1,                   // 672
    S_MUMMY_PAIN2,                   // 673
    S_MUMMY_DIE1,                   // 674
    S_MUMMY_DIE2,                   // 675
    S_MUMMY_DIE3,                   // 676
    S_MUMMY_DIE4,                   // 677
    S_MUMMY_DIE5,                   // 678
    S_MUMMY_DIE6,                   // 679
    S_MUMMY_DIE7,                   // 680
    S_MUMMY_DIE8,                   // 681
    S_MUMMY_SOUL1,                   // 682
    S_MUMMY_SOUL2,                   // 683
    S_MUMMY_SOUL3,                   // 684
    S_MUMMY_SOUL4,                   // 685
    S_MUMMY_SOUL5,                   // 686
    S_MUMMY_SOUL6,                   // 687
    S_MUMMY_SOUL7,                   // 688
    S_MUMMYFX1_1,                   // 689
    S_MUMMYFX1_2,                   // 690
    S_MUMMYFX1_3,                   // 691
    S_MUMMYFX1_4,                   // 692
    S_MUMMYFXI1_1,                   // 693
    S_MUMMYFXI1_2,                   // 694
    S_MUMMYFXI1_3,                   // 695
    S_MUMMYFXI1_4,                   // 696
    S_BEAST_LOOK1,                   // 697
    S_BEAST_LOOK2,                   // 698
    S_BEAST_WALK1,                   // 699
    S_BEAST_WALK2,                   // 700
    S_BEAST_WALK3,                   // 701
    S_BEAST_WALK4,                   // 702
    S_BEAST_WALK5,                   // 703
    S_BEAST_WALK6,                   // 704
    S_BEAST_ATK1,                   // 705
    S_BEAST_ATK2,                   // 706
    S_BEAST_PAIN1,                   // 707
    S_BEAST_PAIN2,                   // 708
    S_BEAST_DIE1,                   // 709
    S_BEAST_DIE2,                   // 710
    S_BEAST_DIE3,                   // 711
    S_BEAST_DIE4,                   // 712
    S_BEAST_DIE5,                   // 713
    S_BEAST_DIE6,                   // 714
    S_BEAST_DIE7,                   // 715
    S_BEAST_DIE8,                   // 716
    S_BEAST_DIE9,                   // 717
    S_BEAST_XDIE1,                   // 718
    S_BEAST_XDIE2,                   // 719
    S_BEAST_XDIE3,                   // 720
    S_BEAST_XDIE4,                   // 721
    S_BEAST_XDIE5,                   // 722
    S_BEAST_XDIE6,                   // 723
    S_BEAST_XDIE7,                   // 724
    S_BEAST_XDIE8,                   // 725
    S_BEASTBALL1,                   // 726
    S_BEASTBALL2,                   // 727
    S_BEASTBALL3,                   // 728
    S_BEASTBALL4,                   // 729
    S_BEASTBALL5,                   // 730
    S_BEASTBALL6,                   // 731
    S_BEASTBALLX1,                   // 732
    S_BEASTBALLX2,                   // 733
    S_BEASTBALLX3,                   // 734
    S_BEASTBALLX4,                   // 735
    S_BEASTBALLX5,                   // 736
    S_BURNBALL1,                   // 737
    S_BURNBALL2,                   // 738
    S_BURNBALL3,                   // 739
    S_BURNBALL4,                   // 740
    S_BURNBALL5,                   // 741
    S_BURNBALL6,                   // 742
    S_BURNBALL7,                   // 743
    S_BURNBALL8,                   // 744
    S_BURNBALLFB1,                   // 745
    S_BURNBALLFB2,                   // 746
    S_BURNBALLFB3,                   // 747
    S_BURNBALLFB4,                   // 748
    S_BURNBALLFB5,                   // 749
    S_BURNBALLFB6,                   // 750
    S_BURNBALLFB7,                   // 751
    S_BURNBALLFB8,                   // 752
    S_PUFFY1,                       // 753
    S_PUFFY2,                       // 754
    S_PUFFY3,                       // 755
    S_PUFFY4,                       // 756
    S_PUFFY5,                       // 757
    S_SNAKE_LOOK1,                   // 758
    S_SNAKE_LOOK2,                   // 759
    S_SNAKE_WALK1,                   // 760
    S_SNAKE_WALK2,                   // 761
    S_SNAKE_WALK3,                   // 762
    S_SNAKE_WALK4,                   // 763
    S_SNAKE_ATK1,                   // 764
    S_SNAKE_ATK2,                   // 765
    S_SNAKE_ATK3,                   // 766
    S_SNAKE_ATK4,                   // 767
    S_SNAKE_ATK5,                   // 768
    S_SNAKE_ATK6,                   // 769
    S_SNAKE_ATK7,                   // 770
    S_SNAKE_ATK8,                   // 771
    S_SNAKE_ATK9,                   // 772
    S_SNAKE_PAIN1,                   // 773
    S_SNAKE_PAIN2,                   // 774
    S_SNAKE_DIE1,                   // 775
    S_SNAKE_DIE2,                   // 776
    S_SNAKE_DIE3,                   // 777
    S_SNAKE_DIE4,                   // 778
    S_SNAKE_DIE5,                   // 779
    S_SNAKE_DIE6,                   // 780
    S_SNAKE_DIE7,                   // 781
    S_SNAKE_DIE8,                   // 782
    S_SNAKE_DIE9,                   // 783
    S_SNAKE_DIE10,                   // 784
    S_SNAKEPRO_A1,                   // 785
    S_SNAKEPRO_A2,                   // 786
    S_SNAKEPRO_A3,                   // 787
    S_SNAKEPRO_A4,                   // 788
    S_SNAKEPRO_AX1,                   // 789
    S_SNAKEPRO_AX2,                   // 790
    S_SNAKEPRO_AX3,                   // 791
    S_SNAKEPRO_AX4,                   // 792
    S_SNAKEPRO_AX5,                   // 793
    S_SNAKEPRO_B1,                   // 794
    S_SNAKEPRO_B2,                   // 795
    S_SNAKEPRO_BX1,                   // 796
    S_SNAKEPRO_BX2,                   // 797
    S_SNAKEPRO_BX3,                   // 798
    S_SNAKEPRO_BX4,                   // 799
    S_HEAD_LOOK,                   // 800
    S_HEAD_FLOAT,                   // 801
    S_HEAD_ATK1,                   // 802
    S_HEAD_ATK2,                   // 803
    S_HEAD_PAIN1,                   // 804
    S_HEAD_PAIN2,                   // 805
    S_HEAD_DIE1,                   // 806
    S_HEAD_DIE2,                   // 807
    S_HEAD_DIE3,                   // 808
    S_HEAD_DIE4,                   // 809
    S_HEAD_DIE5,                   // 810
    S_HEAD_DIE6,                   // 811
    S_HEAD_DIE7,                   // 812
    S_HEADFX1_1,                   // 813
    S_HEADFX1_2,                   // 814
    S_HEADFX1_3,                   // 815
    S_HEADFXI1_1,                   // 816
    S_HEADFXI1_2,                   // 817
    S_HEADFXI1_3,                   // 818
    S_HEADFXI1_4,                   // 819
    S_HEADFX2_1,                   // 820
    S_HEADFX2_2,                   // 821
    S_HEADFX2_3,                   // 822
    S_HEADFXI2_1,                   // 823
    S_HEADFXI2_2,                   // 824
    S_HEADFXI2_3,                   // 825
    S_HEADFXI2_4,                   // 826
    S_HEADFX3_1,                   // 827
    S_HEADFX3_2,                   // 828
    S_HEADFX3_3,                   // 829
    S_HEADFX3_4,                   // 830
    S_HEADFX3_5,                   // 831
    S_HEADFX3_6,                   // 832
    S_HEADFXI3_1,                   // 833
    S_HEADFXI3_2,                   // 834
    S_HEADFXI3_3,                   // 835
    S_HEADFXI3_4,                   // 836
    S_HEADFX4_1,                   // 837
    S_HEADFX4_2,                   // 838
    S_HEADFX4_3,                   // 839
    S_HEADFX4_4,                   // 840
    S_HEADFX4_5,                   // 841
    S_HEADFX4_6,                   // 842
    S_HEADFX4_7,                   // 843
    S_HEADFXI4_1,                   // 844
    S_HEADFXI4_2,                   // 845
    S_HEADFXI4_3,                   // 846
    S_HEADFXI4_4,                   // 847
    S_CLINK_LOOK1,                   // 848
    S_CLINK_LOOK2,                   // 849
    S_CLINK_WALK1,                   // 850
    S_CLINK_WALK2,                   // 851
    S_CLINK_WALK3,                   // 852
    S_CLINK_WALK4,                   // 853
    S_CLINK_ATK1,                   // 854
    S_CLINK_ATK2,                   // 855
    S_CLINK_ATK3,                   // 856
    S_CLINK_PAIN1,                   // 857
    S_CLINK_PAIN2,                   // 858
    S_CLINK_DIE1,                   // 859
    S_CLINK_DIE2,                   // 860
    S_CLINK_DIE3,                   // 861
    S_CLINK_DIE4,                   // 862
    S_CLINK_DIE5,                   // 863
    S_CLINK_DIE6,                   // 864
    S_CLINK_DIE7,                   // 865
    S_WIZARD_LOOK1,                   // 866
    S_WIZARD_LOOK2,                   // 867
    S_WIZARD_WALK1,                   // 868
    S_WIZARD_WALK2,                   // 869
    S_WIZARD_WALK3,                   // 870
    S_WIZARD_WALK4,                   // 871
    S_WIZARD_WALK5,                   // 872
    S_WIZARD_WALK6,                   // 873
    S_WIZARD_WALK7,                   // 874
    S_WIZARD_WALK8,                   // 875
    S_WIZARD_ATK1,                   // 876
    S_WIZARD_ATK2,                   // 877
    S_WIZARD_ATK3,                   // 878
    S_WIZARD_ATK4,                   // 879
    S_WIZARD_ATK5,                   // 880
    S_WIZARD_ATK6,                   // 881
    S_WIZARD_ATK7,                   // 882
    S_WIZARD_ATK8,                   // 883
    S_WIZARD_ATK9,                   // 884
    S_WIZARD_PAIN1,                   // 885
    S_WIZARD_PAIN2,                   // 886
    S_WIZARD_DIE1,                   // 887
    S_WIZARD_DIE2,                   // 888
    S_WIZARD_DIE3,                   // 889
    S_WIZARD_DIE4,                   // 890
    S_WIZARD_DIE5,                   // 891
    S_WIZARD_DIE6,                   // 892
    S_WIZARD_DIE7,                   // 893
    S_WIZARD_DIE8,                   // 894
    S_WIZFX1_1,                       // 895
    S_WIZFX1_2,                       // 896
    S_WIZFXI1_1,                   // 897
    S_WIZFXI1_2,                   // 898
    S_WIZFXI1_3,                   // 899
    S_WIZFXI1_4,                   // 900
    S_WIZFXI1_5,                   // 901
    S_IMP_LOOK1,                   // 902
    S_IMP_LOOK2,                   // 903
    S_IMP_LOOK3,                   // 904
    S_IMP_LOOK4,                   // 905
    S_IMP_FLY1,                       // 906
    S_IMP_FLY2,                       // 907
    S_IMP_FLY3,                       // 908
    S_IMP_FLY4,                       // 909
    S_IMP_FLY5,                       // 910
    S_IMP_FLY6,                       // 911
    S_IMP_FLY7,                       // 912
    S_IMP_FLY8,                       // 913
    S_IMP_MEATK1,                   // 914
    S_IMP_MEATK2,                   // 915
    S_IMP_MEATK3,                   // 916
    S_IMP_MSATK1_1,                   // 917
    S_IMP_MSATK1_2,                   // 918
    S_IMP_MSATK1_3,                   // 919
    S_IMP_MSATK1_4,                   // 920
    S_IMP_MSATK1_5,                   // 921
    S_IMP_MSATK1_6,                   // 922
    S_IMP_MSATK2_1,                   // 923
    S_IMP_MSATK2_2,                   // 924
    S_IMP_MSATK2_3,                   // 925
    S_IMP_PAIN1,                   // 926
    S_IMP_PAIN2,                   // 927
    S_IMP_DIE1,                       // 928
    S_IMP_DIE2,                       // 929
    S_IMP_XDIE1,                   // 930
    S_IMP_XDIE2,                   // 931
    S_IMP_XDIE3,                   // 932
    S_IMP_XDIE4,                   // 933
    S_IMP_XDIE5,                   // 934
    S_IMP_CRASH1,                   // 935
    S_IMP_CRASH2,                   // 936
    S_IMP_CRASH3,                   // 937
    S_IMP_CRASH4,                   // 938
    S_IMP_XCRASH1,                   // 939
    S_IMP_XCRASH2,                   // 940
    S_IMP_XCRASH3,                   // 941
    S_IMP_CHUNKA1,                   // 942
    S_IMP_CHUNKA2,                   // 943
    S_IMP_CHUNKA3,                   // 944
    S_IMP_CHUNKB1,                   // 945
    S_IMP_CHUNKB2,                   // 946
    S_IMP_CHUNKB3,                   // 947
    S_IMPFX1,                       // 948
    S_IMPFX2,                       // 949
    S_IMPFX3,                       // 950
    S_IMPFXI1,                       // 951
    S_IMPFXI2,                       // 952
    S_IMPFXI3,                       // 953
    S_IMPFXI4,                       // 954
    S_KNIGHT_STND1,                   // 955
    S_KNIGHT_STND2,                   // 956
    S_KNIGHT_WALK1,                   // 957
    S_KNIGHT_WALK2,                   // 958
    S_KNIGHT_WALK3,                   // 959
    S_KNIGHT_WALK4,                   // 960
    S_KNIGHT_ATK1,                   // 961
    S_KNIGHT_ATK2,                   // 962
    S_KNIGHT_ATK3,                   // 963
    S_KNIGHT_ATK4,                   // 964
    S_KNIGHT_ATK5,                   // 965
    S_KNIGHT_ATK6,                   // 966
    S_KNIGHT_PAIN1,                   // 967
    S_KNIGHT_PAIN2,                   // 968
    S_KNIGHT_DIE1,                   // 969
    S_KNIGHT_DIE2,                   // 970
    S_KNIGHT_DIE3,                   // 971
    S_KNIGHT_DIE4,                   // 972
    S_KNIGHT_DIE5,                   // 973
    S_KNIGHT_DIE6,                   // 974
    S_KNIGHT_DIE7,                   // 975
    S_SPINAXE1,                       // 976
    S_SPINAXE2,                       // 977
    S_SPINAXE3,                       // 978
    S_SPINAXEX1,                   // 979
    S_SPINAXEX2,                   // 980
    S_SPINAXEX3,                   // 981
    S_REDAXE1,                       // 982
    S_REDAXE2,                       // 983
    S_REDAXEX1,                       // 984
    S_REDAXEX2,                       // 985
    S_REDAXEX3,                       // 986
    S_SRCR1_LOOK1,                   // 987
    S_SRCR1_LOOK2,                   // 988
    S_SRCR1_WALK1,                   // 989
    S_SRCR1_WALK2,                   // 990
    S_SRCR1_WALK3,                   // 991
    S_SRCR1_WALK4,                   // 992
    S_SRCR1_PAIN1,                   // 993
    S_SRCR1_ATK1,                   // 994
    S_SRCR1_ATK2,                   // 995
    S_SRCR1_ATK3,                   // 996
    S_SRCR1_ATK4,                   // 997
    S_SRCR1_ATK5,                   // 998
    S_SRCR1_ATK6,                   // 999
    S_SRCR1_ATK7,                   // 1000
    S_SRCR1_DIE1,                   // 1001
    S_SRCR1_DIE2,                   // 1002
    S_SRCR1_DIE3,                   // 1003
    S_SRCR1_DIE4,                   // 1004
    S_SRCR1_DIE5,                   // 1005
    S_SRCR1_DIE6,                   // 1006
    S_SRCR1_DIE7,                   // 1007
    S_SRCR1_DIE8,                   // 1008
    S_SRCR1_DIE9,                   // 1009
    S_SRCR1_DIE10,                   // 1010
    S_SRCR1_DIE11,                   // 1011
    S_SRCR1_DIE12,                   // 1012
    S_SRCR1_DIE13,                   // 1013
    S_SRCR1_DIE14,                   // 1014
    S_SRCR1_DIE15,                   // 1015
    S_SRCR1_DIE16,                   // 1016
    S_SRCR1_DIE17,                   // 1017
    S_SRCRFX1_1,                   // 1018
    S_SRCRFX1_2,                   // 1019
    S_SRCRFX1_3,                   // 1020
    S_SRCRFXI1_1,                   // 1021
    S_SRCRFXI1_2,                   // 1022
    S_SRCRFXI1_3,                   // 1023
    S_SRCRFXI1_4,                   // 1024
    S_SRCRFXI1_5,                   // 1025
    S_SOR2_RISE1,                   // 1026
    S_SOR2_RISE2,                   // 1027
    S_SOR2_RISE3,                   // 1028
    S_SOR2_RISE4,                   // 1029
    S_SOR2_RISE5,                   // 1030
    S_SOR2_RISE6,                   // 1031
    S_SOR2_RISE7,                   // 1032
    S_SOR2_LOOK1,                   // 1033
    S_SOR2_LOOK2,                   // 1034
    S_SOR2_WALK1,                   // 1035
    S_SOR2_WALK2,                   // 1036
    S_SOR2_WALK3,                   // 1037
    S_SOR2_WALK4,                   // 1038
    S_SOR2_PAIN1,                   // 1039
    S_SOR2_PAIN2,                   // 1040
    S_SOR2_ATK1,                   // 1041
    S_SOR2_ATK2,                   // 1042
    S_SOR2_ATK3,                   // 1043
    S_SOR2_TELE1,                   // 1044
    S_SOR2_TELE2,                   // 1045
    S_SOR2_TELE3,                   // 1046
    S_SOR2_TELE4,                   // 1047
    S_SOR2_TELE5,                   // 1048
    S_SOR2_TELE6,                   // 1049
    S_SOR2_DIE1,                   // 1050
    S_SOR2_DIE2,                   // 1051
    S_SOR2_DIE3,                   // 1052
    S_SOR2_DIE4,                   // 1053
    S_SOR2_DIE5,                   // 1054
    S_SOR2_DIE6,                   // 1055
    S_SOR2_DIE7,                   // 1056
    S_SOR2_DIE8,                   // 1057
    S_SOR2_DIE9,                   // 1058
    S_SOR2_DIE10,                   // 1059
    S_SOR2_DIE11,                   // 1060
    S_SOR2_DIE12,                   // 1061
    S_SOR2_DIE13,                   // 1062
    S_SOR2_DIE14,                   // 1063
    S_SOR2_DIE15,                   // 1064
    S_SOR2FX1_1,                   // 1065
    S_SOR2FX1_2,                   // 1066
    S_SOR2FX1_3,                   // 1067
    S_SOR2FXI1_1,                   // 1068
    S_SOR2FXI1_2,                   // 1069
    S_SOR2FXI1_3,                   // 1070
    S_SOR2FXI1_4,                   // 1071
    S_SOR2FXI1_5,                   // 1072
    S_SOR2FXI1_6,                   // 1073
    S_SOR2FXSPARK1,                   // 1074
    S_SOR2FXSPARK2,                   // 1075
    S_SOR2FXSPARK3,                   // 1076
    S_SOR2FX2_1,                   // 1077
    S_SOR2FX2_2,                   // 1078
    S_SOR2FX2_3,                   // 1079
    S_SOR2FXI2_1,                   // 1080
    S_SOR2FXI2_2,                   // 1081
    S_SOR2FXI2_3,                   // 1082
    S_SOR2FXI2_4,                   // 1083
    S_SOR2FXI2_5,                   // 1084
    S_SOR2TELEFADE1,               // 1085
    S_SOR2TELEFADE2,               // 1086
    S_SOR2TELEFADE3,               // 1087
    S_SOR2TELEFADE4,               // 1088
    S_SOR2TELEFADE5,               // 1089
    S_SOR2TELEFADE6,               // 1090
    S_MNTR_LOOK1,                   // 1091
    S_MNTR_LOOK2,                   // 1092
    S_MNTR_WALK1,                   // 1093
    S_MNTR_WALK2,                   // 1094
    S_MNTR_WALK3,                   // 1095
    S_MNTR_WALK4,                   // 1096
    S_MNTR_ATK1_1,                   // 1097
    S_MNTR_ATK1_2,                   // 1098
    S_MNTR_ATK1_3,                   // 1099
    S_MNTR_ATK2_1,                   // 1100
    S_MNTR_ATK2_2,                   // 1101
    S_MNTR_ATK2_3,                   // 1102
    S_MNTR_ATK3_1,                   // 1103
    S_MNTR_ATK3_2,                   // 1104
    S_MNTR_ATK3_3,                   // 1105
    S_MNTR_ATK3_4,                   // 1106
    S_MNTR_ATK4_1,                   // 1107
    S_MNTR_PAIN1,                   // 1108
    S_MNTR_PAIN2,                   // 1109
    S_MNTR_DIE1,                   // 1110
    S_MNTR_DIE2,                   // 1111
    S_MNTR_DIE3,                   // 1112
    S_MNTR_DIE4,                   // 1113
    S_MNTR_DIE5,                   // 1114
    S_MNTR_DIE6,                   // 1115
    S_MNTR_DIE7,                   // 1116
    S_MNTR_DIE8,                   // 1117
    S_MNTR_DIE9,                   // 1118
    S_MNTR_DIE10,                   // 1119
    S_MNTR_DIE11,                   // 1120
    S_MNTR_DIE12,                   // 1121
    S_MNTR_DIE13,                   // 1122
    S_MNTR_DIE14,                   // 1123
    S_MNTR_DIE15,                   // 1124
    S_MNTRFX1_1,                   // 1125
    S_MNTRFX1_2,                   // 1126
    S_MNTRFXI1_1,                   // 1127
    S_MNTRFXI1_2,                   // 1128
    S_MNTRFXI1_3,                   // 1129
    S_MNTRFXI1_4,                   // 1130
    S_MNTRFXI1_5,                   // 1131
    S_MNTRFXI1_6,                   // 1132
    S_MNTRFX2_1,                   // 1133
    S_MNTRFXI2_1,                   // 1134
    S_MNTRFXI2_2,                   // 1135
    S_MNTRFXI2_3,                   // 1136
    S_MNTRFXI2_4,                   // 1137
    S_MNTRFXI2_5,                   // 1138
    S_MNTRFX3_1,                   // 1139
    S_MNTRFX3_2,                   // 1140
    S_MNTRFX3_3,                   // 1141
    S_MNTRFX3_4,                   // 1142
    S_MNTRFX3_5,                   // 1143
    S_MNTRFX3_6,                   // 1144
    S_MNTRFX3_7,                   // 1145
    S_MNTRFX3_8,                   // 1146
    S_MNTRFX3_9,                   // 1147
    S_AKYY1,                       // 1148
    S_AKYY2,                       // 1149
    S_AKYY3,                       // 1150
    S_AKYY4,                       // 1151
    S_AKYY5,                       // 1152
    S_AKYY6,                       // 1153
    S_AKYY7,                       // 1154
    S_AKYY8,                       // 1155
    S_AKYY9,                       // 1156
    S_AKYY10,                       // 1157
    S_BKYY1,                       // 1158
    S_BKYY2,                       // 1159
    S_BKYY3,                       // 1160
    S_BKYY4,                       // 1161
    S_BKYY5,                       // 1162
    S_BKYY6,                       // 1163
    S_BKYY7,                       // 1164
    S_BKYY8,                       // 1165
    S_BKYY9,                       // 1166
    S_BKYY10,                       // 1167
    S_CKYY1,                       // 1168
    S_CKYY2,                       // 1169
    S_CKYY3,                       // 1170
    S_CKYY4,                       // 1171
    S_CKYY5,                       // 1172
    S_CKYY6,                       // 1173
    S_CKYY7,                       // 1174
    S_CKYY8,                       // 1175
    S_CKYY9,                       // 1176
    S_AMG1,                           // 1177
    S_AMG2_1,                       // 1178
    S_AMG2_2,                       // 1179
    S_AMG2_3,                       // 1180
    S_AMM1,                           // 1181
    S_AMM2,                           // 1182
    S_AMC1,                           // 1183
    S_AMC2_1,                       // 1184
    S_AMC2_2,                       // 1185
    S_AMC2_3,                       // 1186
    S_AMS1_1,                       // 1187
    S_AMS1_2,                       // 1188
    S_AMS2_1,                       // 1189
    S_AMS2_2,                       // 1190
    S_AMP1_1,                       // 1191
    S_AMP1_2,                       // 1192
    S_AMP1_3,                       // 1193
    S_AMP2_1,                       // 1194
    S_AMP2_2,                       // 1195
    S_AMP2_3,                       // 1196
    S_AMB1_1,                       // 1197
    S_AMB1_2,                       // 1198
    S_AMB1_3,                       // 1199
    S_AMB2_1,                       // 1200
    S_AMB2_2,                       // 1201
    S_AMB2_3,                       // 1202
    S_SND_WIND,                       // 1203
    S_SND_WATERFALL,               // 1204
    S_TEMPSOUNDORIGIN1,               // 1205
    NUMSTATES                       // 1206
} statenum_t;

// Map objects.
typedef enum {
    MT_MISC0,                       // 000
    MT_ITEMSHIELD1,                   // 001
    MT_ITEMSHIELD2,                   // 002
    MT_MISC1,                       // 003
    MT_MISC2,                       // 004
    MT_ARTIINVISIBILITY,           // 005
    MT_MISC3,                       // 006
    MT_ARTIFLY,                       // 007
    MT_ARTIINVULNERABILITY,           // 008
    MT_ARTITOMEOFPOWER,               // 009
    MT_ARTIEGG,                       // 010
    MT_EGGFX,                       // 011
    MT_ARTISUPERHEAL,               // 012
    MT_MISC4,                       // 013
    MT_MISC5,                       // 014
    MT_FIREBOMB,                   // 015
    MT_ARTITELEPORT,               // 016
    MT_POD,                           // 017
    MT_PODGOO,                       // 018
    MT_PODGENERATOR,               // 019
    MT_SPLASH,                       // 020
    MT_SPLASHBASE,                   // 021
    MT_LAVASPLASH,                   // 022
    MT_LAVASMOKE,                   // 023
    MT_SLUDGECHUNK,                   // 024
    MT_SLUDGESPLASH,               // 025
    MT_SKULLHANG70,                   // 026
    MT_SKULLHANG60,                   // 027
    MT_SKULLHANG45,                   // 028
    MT_SKULLHANG35,                   // 029
    MT_CHANDELIER,                   // 030
    MT_SERPTORCH,                   // 031
    MT_SMALLPILLAR,                   // 032
    MT_STALAGMITESMALL,               // 033
    MT_STALAGMITELARGE,               // 034
    MT_STALACTITESMALL,               // 035
    MT_STALACTITELARGE,               // 036
    MT_MISC6,                       // 037
    MT_BARREL,                       // 038
    MT_MISC7,                       // 039
    MT_MISC8,                       // 040
    MT_MISC9,                       // 041
    MT_MISC10,                       // 042
    MT_MISC11,                       // 043
    MT_KEYGIZMOBLUE,               // 044
    MT_KEYGIZMOGREEN,               // 045
    MT_KEYGIZMOYELLOW,               // 046
    MT_KEYGIZMOFLOAT,               // 047
    MT_MISC12,                       // 048
    MT_VOLCANOBLAST,               // 049
    MT_VOLCANOTBLAST,               // 050
    MT_TELEGLITGEN,                   // 051
    MT_TELEGLITGEN2,               // 052
    MT_TELEGLITTER,                   // 053
    MT_TELEGLITTER2,               // 054
    MT_TFOG,                       // 055
    MT_TELEPORTMAN,                   // 056
    MT_STAFFPUFF,                   // 057
    MT_STAFFPUFF2,                   // 058
    MT_BEAKPUFF,                   // 059
    MT_MISC13,                       // 060
    MT_GAUNTLETPUFF1,               // 061
    MT_GAUNTLETPUFF2,               // 062
    MT_MISC14,                       // 063
    MT_BLASTERFX1,                   // 064
    MT_BLASTERSMOKE,               // 065
    MT_RIPPER,                       // 066
    MT_BLASTERPUFF1,               // 067
    MT_BLASTERPUFF2,               // 068
    MT_WMACE,                       // 069
    MT_MACEFX1,                       // 070
    MT_MACEFX2,                       // 071
    MT_MACEFX3,                       // 072
    MT_MACEFX4,                       // 073
    MT_WSKULLROD,                   // 074
    MT_HORNRODFX1,                   // 075
    MT_HORNRODFX2,                   // 076
    MT_RAINPLR1,                   // 077
    MT_RAINPLR2,                   // 078
    MT_RAINPLR3,                   // 079
    MT_RAINPLR4,                   // 080
    MT_GOLDWANDFX1,                   // 081
    MT_GOLDWANDFX2,                   // 082
    MT_GOLDWANDPUFF1,               // 083
    MT_GOLDWANDPUFF2,               // 084
    MT_WPHOENIXROD,                   // 085
    MT_PHOENIXFX1,                   // 086
    MT_PHOENIXPUFF,                   // 087
    MT_PHOENIXFX2,                   // 088
    MT_MISC15,                       // 089
    MT_CRBOWFX1,                   // 090
    MT_CRBOWFX2,                   // 091
    MT_CRBOWFX3,                   // 092
    MT_CRBOWFX4,                   // 093
    MT_BLOOD,                       // 094
    MT_BLOODSPLATTER,               // 095
    MT_PLAYER,                       // 096
    MT_BLOODYSKULL,                   // 097
    MT_CHICPLAYER,                   // 098
    MT_CHICKEN,                       // 099
    MT_FEATHER,                       // 100
    MT_MUMMY,                       // 101
    MT_MUMMYLEADER,                   // 102
    MT_MUMMYGHOST,                   // 103
    MT_MUMMYLEADERGHOST,           // 104
    MT_MUMMYSOUL,                   // 105
    MT_MUMMYFX1,                   // 106
    MT_BEAST,                       // 107
    MT_BEASTBALL,                   // 108
    MT_BURNBALL,                   // 109
    MT_BURNBALLFB,                   // 110
    MT_PUFFY,                       // 111
    MT_SNAKE,                       // 112
    MT_SNAKEPRO_A,                   // 113
    MT_SNAKEPRO_B,                   // 114
    MT_HEAD,                       // 115
    MT_HEADFX1,                       // 116
    MT_HEADFX2,                       // 117
    MT_HEADFX3,                       // 118
    MT_WHIRLWIND,                   // 119
    MT_CLINK,                       // 120
    MT_WIZARD,                       // 121
    MT_WIZFX1,                       // 122
    MT_IMP,                           // 123
    MT_IMPLEADER,                   // 124
    MT_IMPCHUNK1,                   // 125
    MT_IMPCHUNK2,                   // 126
    MT_IMPBALL,                       // 127
    MT_KNIGHT,                       // 128
    MT_KNIGHTGHOST,                   // 129
    MT_KNIGHTAXE,                   // 130
    MT_REDAXE,                       // 131
    MT_SORCERER1,                   // 132
    MT_SRCRFX1,                       // 133
    MT_SORCERER2,                   // 134
    MT_SOR2FX1,                       // 135
    MT_SOR2FXSPARK,                   // 136
    MT_SOR2FX2,                       // 137
    MT_SOR2TELEFADE,               // 138
    MT_MINOTAUR,                   // 139
    MT_MNTRFX1,                       // 140
    MT_MNTRFX2,                       // 141
    MT_MNTRFX3,                       // 142
    MT_AKYY,                       // 143
    MT_BKYY,                       // 144
    MT_CKEY,                       // 145
    MT_AMGWNDWIMPY,                   // 146
    MT_AMGWNDHEFTY,                   // 147
    MT_AMMACEWIMPY,                   // 148
    MT_AMMACEHEFTY,                   // 149
    MT_AMCBOWWIMPY,                   // 150
    MT_AMCBOWHEFTY,                   // 151
    MT_AMSKRDWIMPY,                   // 152
    MT_AMSKRDHEFTY,                   // 153
    MT_AMPHRDWIMPY,                   // 154
    MT_AMPHRDHEFTY,                   // 155
    MT_AMBLSRWIMPY,                   // 156
    MT_AMBLSRHEFTY,                   // 157
    MT_SOUNDWIND,                   // 158
    MT_SOUNDWATERFALL,               // 159
    MT_TEMPSOUNDORIGIN,               // 160
    NUMMOBJTYPES                   // 161
} mobjtype_t;

// Text.
typedef enum {
    TXT_PRESSKEY,
    TXT_PRESSYN,
    TXT_TXT_PAUSED,
    TXT_QUITMSG,
    TXT_LOADNET,
    TXT_QLOADNET,
    TXT_QSAVESPOT,
    TXT_SAVEDEAD,                 // 30
    TXT_QSPROMPT,
    TXT_QLPROMPT,
    TXT_NEWGAME,
    TXT_NIGHTMARE,
    TXT_SWSTRING,
    TXT_MSGOFF,
    TXT_MSGON,
    TXT_NETEND,
    TXT_ENDGAME,
    TXT_DOSY,
    TXT_DETAILHI,
    TXT_DETAILLO,
    TXT_GAMMALVL0,
    TXT_GAMMALVL1,
    TXT_GAMMALVL2,
    TXT_GAMMALVL3,
    TXT_GAMMALVL4,
    TXT_EMPTYSTRING,
    TXT_TXT_GOTBLUEKEY,
    TXT_TXT_GOTYELLOWKEY,
    TXT_TXT_GOTGREENKEY,
    TXT_TXT_ARTIHEALTH,
    TXT_TXT_ARTIFLY,
    TXT_TXT_ARTIINVULNERABILITY,
    TXT_TXT_ARTITOMEOFPOWER,
    TXT_TXT_ARTIINVISIBILITY,
    TXT_TXT_ARTIEGG,
    TXT_TXT_ARTISUPERHEALTH,
    TXT_TXT_ARTITORCH,
    TXT_TXT_ARTIFIREBOMB,
    TXT_TXT_ARTITELEPORT,
    TXT_TXT_ITEMHEALTH,
    TXT_TXT_ITEMBAGOFHOLDING,
    TXT_TXT_ITEMSHIELD1,
    TXT_TXT_ITEMSHIELD2,
    TXT_TXT_ITEMSUPERMAP,
    TXT_TXT_AMMOGOLDWAND1,
    TXT_TXT_AMMOGOLDWAND2,
    TXT_TXT_AMMOMACE1,
    TXT_TXT_AMMOMACE2,
    TXT_TXT_AMMOCROSSBOW1,
    TXT_TXT_AMMOCROSSBOW2,
    TXT_TXT_AMMOBLASTER1,
    TXT_TXT_AMMOBLASTER2,
    TXT_TXT_AMMOSKULLROD1,
    TXT_TXT_AMMOSKULLROD2,
    TXT_TXT_AMMOPHOENIXROD1,
    TXT_TXT_AMMOPHOENIXROD2,
    TXT_TXT_WPNSTAFF,
    TXT_TXT_WPNWAND,
    TXT_TXT_WPNCROSSBOW,
    TXT_TXT_WPNBLASTER,
    TXT_TXT_WPNSKULLROD,
    TXT_TXT_WPNPHOENIXROD,
    TXT_TXT_WPNMACE,
    TXT_TXT_WPNGAUNTLETS,
    TXT_TXT_WPNBEAK,
    TXT_TXT_CHEATGODON,
    TXT_TXT_CHEATGODOFF,
    TXT_TXT_CHEATNOCLIPON,
    TXT_TXT_CHEATNOCLIPOFF,
    TXT_TXT_CHEATWEAPONS,
    TXT_TXT_CHEATFLIGHTON,
    TXT_TXT_CHEATFLIGHTOFF,
    TXT_TXT_CHEATPOWERON,
    TXT_TXT_CHEATPOWEROFF,
    TXT_TXT_CHEATHEALTH,
    TXT_TXT_CHEATKEYS,
    TXT_TXT_CHEATSOUNDON,
    TXT_TXT_CHEATSOUNDOFF,
    TXT_TXT_CHEATTICKERON,
    TXT_TXT_CHEATTICKEROFF,
    TXT_TXT_CHEATARTIFACTS1,
    TXT_TXT_CHEATARTIFACTS2,
    TXT_TXT_CHEATARTIFACTS3,
    TXT_TXT_CHEATARTIFACTSFAIL,
    TXT_TXT_CHEATWARP,
    TXT_TXT_CHEATSCREENSHOT,
    TXT_TXT_CHEATCHICKENON,
    TXT_TXT_CHEATCHICKENOFF,
    TXT_TXT_CHEATMASSACRE,
    TXT_TXT_CHEATIDDQD,
    TXT_TXT_CHEATIDKFA,
    TXT_TXT_NEEDBLUEKEY,
    TXT_TXT_NEEDGREENKEY,
    TXT_TXT_NEEDYELLOWKEY,
    TXT_TXT_GAMESAVED,
    TXT_HUSTR_CHATMACRO0,
    TXT_HUSTR_CHATMACRO1,
    TXT_HUSTR_CHATMACRO2,
    TXT_HUSTR_CHATMACRO3,
    TXT_HUSTR_CHATMACRO4,
    TXT_HUSTR_CHATMACRO5,
    TXT_HUSTR_CHATMACRO6,
    TXT_HUSTR_CHATMACRO7,
    TXT_HUSTR_CHATMACRO8,
    TXT_HUSTR_CHATMACRO9,
    TXT_HUSTR_TALKTOSELF1,
    TXT_HUSTR_TALKTOSELF2,
    TXT_HUSTR_TALKTOSELF3,
    TXT_HUSTR_TALKTOSELF4,
    TXT_HUSTR_TALKTOSELF5,
    TXT_HUSTR_MESSAGESENT,
    TXT_HUSTR_PLRGREEN,
    TXT_HUSTR_PLRINDIGO,
    TXT_HUSTR_PLRBROWN,
    TXT_HUSTR_PLRRED,
    TXT_AMSTR_FOLLOWON,
    TXT_AMSTR_FOLLOWOFF,
    TXT_AMSTR_GRIDON,
    TXT_AMSTR_GRIDOFF,
    TXT_AMSTR_MARKEDSPOT,
    TXT_AMSTR_MARKSCLEARED,
    TXT_STSTR_DQDON,
    TXT_STSTR_DQDOFF,
    TXT_STSTR_KFAADDED,
    TXT_STSTR_NCON,
    TXT_STSTR_NCOFF,
    TXT_STSTR_BEHOLD,
    TXT_STSTR_BEHOLDX,
    TXT_STSTR_CHOPPERS,
    TXT_STSTR_CLEV,
    TXT_E1TEXT,
    TXT_E2TEXT,
    TXT_E3TEXT,
    TXT_E4TEXT,
    TXT_E5TEXT,
    TXT_LOADMISSING,
    TXT_EPISODE1,
    TXT_EPISODE2,
    TXT_EPISODE3,
    TXT_EPISODE4,
    TXT_EPISODE5,
    TXT_AMSTR_ROTATEON,
    TXT_AMSTR_ROTATEOFF,
    NUMTEXT
} textenum_t;

#endif
