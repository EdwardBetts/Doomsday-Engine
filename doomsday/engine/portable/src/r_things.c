/**\file
 *\section License
 * License: GPL
 * Online License Link: http://www.gnu.org/licenses/gpl.html
 *
 *\author Copyright © 2003-2007 Jaakko Keränen <skyjake@dengine.net>
 *\author Copyright © 2006 Daniel Swanson <danij@dengine.net>
 *\author Copyright © 2006 Jamie Jones <yagisan@dengine.net>
 *\author Copyright © 1993-1996 by id Software, Inc.
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
 * r_things.c: Object Management and Refresh
 */

/*
 * Sprite rotation 0 is facing the viewer, rotation 1 is one angle
 * turn CLOCKWISE around the axis. This is not the same as the angle,
 * which increases counter clockwise (protractor).
 */

// HEADER FILES ------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "de_base.h"
#include "de_console.h"
#include "de_network.h"
#include "de_refresh.h"
#include "de_play.h"
#include "de_render.h"
#include "de_graphics.h"
#include "de_misc.h"

#include "def_main.h"

// MACROS ------------------------------------------------------------------

#define MAX_FRAMES 128
#define MAX_OBJECT_RADIUS 128

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

float   weaponOffsetScale = 0.3183f;    // 1/Pi
int     weaponOffsetScaleY = 1000;
float   weaponFOVShift = 45;
float   modelSpinSpeed = 1;
int     alwaysAlign = 0;
int     noSpriteZWrite = false;
int     pspOffX = 0, pspOffY = 0;
// srvo1 = models only, srvo2 = sprites + models
int     useSRVO = 2, useSRVOAngle = true;

int     psp3d;

// variables used to look up and range check thing_t sprites patches
spritedef_t *sprites = 0;
int     numSprites;

spritelump_t **spritelumps;
int     numSpriteLumps;

vissprite_t vissprites[MAXVISSPRITES], *vissprite_p;
vissprite_t vispsprites[DDMAXPSPRITES];

int     maxModelDistance = 1500;

int     levelFullBright = false;

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static spriteframe_t sprtemp[MAX_FRAMES];
static int     maxFrame;
static char   *spriteName;

static vissprite_t overflowSprite;
static mobj_t *projectedThing;  // Used during RIT_VisMobjZ

// CODE --------------------------------------------------------------------

void R_InitSpriteLumps(void)
{
    lumppatch_t *patch;
    spritelump_t *sl;
    int     i;
    char    buf[64];

    sprintf(buf, "R_Init: Initializing %i sprites...", numSpriteLumps);
    //Con_InitProgress(buf, numSpriteLumps);

    for(i = 0; i < numSpriteLumps; ++i)
    {
        sl = spritelumps[i];

        /*
        if(!(i % 50))
            Con_Progress(i, PBARF_SET | PBARF_DONTSHOW);*/

        patch = (lumppatch_t *) W_CacheLumpNum(sl->lump, PU_CACHE);
        sl->width = SHORT(patch->width);
        sl->height = SHORT(patch->height);
        sl->offset = SHORT(patch->leftoffset);
        sl->topoffset = SHORT(patch->topoffset);
    }
}

/*
 * Returns the new sprite lump number.
 */
int R_NewSpriteLump(int lump)
{
    spritelump_t **newlist, *ptr;
    int     i;

    // Is this lump already entered?
    for(i = 0; i < numSpriteLumps; i++)
        if(spritelumps[i]->lump == lump)
            return i;

    newlist = Z_Malloc(sizeof(spritelump_t*) * ++numSpriteLumps, PU_SPRITE, 0);
    if(numSpriteLumps > 1)
    {
        for(i = 0; i < numSpriteLumps -1; ++i)
            newlist[i] = spritelumps[i];

        Z_Free(spritelumps);
    }
    spritelumps = newlist;
    ptr = spritelumps[numSpriteLumps - 1] = Z_Calloc(sizeof(spritelump_t), PU_SPRITE, 0);
    ptr->lump = lump;
    return numSpriteLumps - 1;
}

/*
 * Local function for R_InitSprites.
 */
void R_InstallSpriteLump(int lump, unsigned frame, unsigned rotation,
                         boolean flipped)
{
    int     splump = R_NewSpriteLump(lump);
    int     r;

    if(frame >= 30 || rotation > 8)
        //Con_Error ("R_InstallSpriteLump: Bad frame characters in lump %i", lump);
        return;

    if((int) frame > maxFrame)
        maxFrame = frame;

    if(rotation == 0)
    {
        // The lump should be used for all rotations.
        /*      if(sprtemp[frame].rotate == false)
           Con_Error ("R_InitSprites: Sprite %s frame %c has multip rot=0 lump",
           spriteName, 'A'+frame);
           if (sprtemp[frame].rotate == true)
           Con_Error ("R_InitSprites: Sprite %s frame %c has rotations and a rot=0 lump"
           , spriteName, 'A'+frame); */

        sprtemp[frame].rotate = false;
        for(r = 0; r < 8; r++)
        {
            sprtemp[frame].lump[r] = splump;    //lump - firstspritelump;
            sprtemp[frame].flip[r] = (byte) flipped;
        }
        return;
    }

    // the lump is only used for one rotation
    /*  if (sprtemp[frame].rotate == false)
       Con_Error ("R_InitSprites: Sprite %s frame %c has rotations and a rot=0 lump"
       , spriteName, 'A'+frame); */

    sprtemp[frame].rotate = true;

    rotation--;                 // make 0 based
    /*  if(sprtemp[frame].lump[rotation] != -1)
       Con_Error ("R_InitSprites: Sprite %s : %c : %c has two lumps mapped to it"
       ,spriteName, 'A'+frame, '1'+rotation); */

    sprtemp[frame].lump[rotation] = splump; //lump - firstspritelump;
    sprtemp[frame].flip[rotation] = (byte) flipped;
}

/*
 * Pass a null terminated list of sprite names (4 chars exactly) to be used
 * Builds the sprite rotation matrixes to account for horizontally flipped
 * sprites.  Will report an error if the lumps are inconsistant.
 *
 * Sprite lump names are 4 characters for the actor, a letter for the frame,
 * and a number for the rotation, A sprite that is flippable will have an
 * additional letter/number appended.  The rotation character can be 0 to
 * signify no rotations.
 */
void R_InitSpriteDefs(void)
{
    int     i, l, intname, frame, rotation;
    boolean in_sprite_block;

    numSpriteLumps = 0;
    numSprites = count_sprnames.num;

    // Check that some sprites are defined.
    if(!numSprites)
        return;

    sprites = Z_Malloc(numSprites * sizeof(*sprites), PU_SPRITE, NULL);

    // Scan all the lump names for each of the names, noting the highest
    // frame letter. Just compare 4 characters as ints.
    for(i = 0; i < numSprites; i++)
    {
        spriteName = sprnames[i].name;
        memset(sprtemp, -1, sizeof(sprtemp));

        maxFrame = -1;
        intname = *(int *) spriteName;

        //
        // scan the lumps, filling in the frames for whatever is found
        //
        in_sprite_block = false;
        for(l = 0; l < numlumps; l++)
        {
            char   *name = lumpinfo[l].name;

            if(!strnicmp(name, "S_START", 7))
            {
                // We've arrived at *a* sprite block.
                in_sprite_block = true;
                continue;
            }
            else if(!strnicmp(name, "S_END", 5))
            {
                // The sprite block ends.
                in_sprite_block = false;
                continue;
            }
            if(!in_sprite_block)
                continue;
            // Check that the first four letters match the sprite name.
            if(*(int *) name == intname)
            {
                // Check that the name is valid.
                if(!name[4] || !name[5] || (name[6] && !name[7]))
                    continue;   // This is not a sprite frame.
                // Indices 5 and 7 must be numbers (0-8).
                if(name[5] < '0' || name[5] > '8')
                    continue;
                if(name[7] && (name[7] < '0' || name[7] > '8'))
                    continue;
                frame = name[4] - 'A';
                rotation = name[5] - '0';
                R_InstallSpriteLump(l, frame, rotation, false);
                if(name[6])
                {
                    frame = name[6] - 'A';
                    rotation = name[7] - '0';
                    R_InstallSpriteLump(l, frame, rotation, true);
                }
            }
        }

        //
        // check the frames that were found for completeness
        //
        if(maxFrame == -1)
        {
            sprites[i].numframes = 0;
            //Con_Error ("R_InitSprites: No lumps found for sprite %s", namelist[i]);
        }

        maxFrame++;
        for(frame = 0; frame < maxFrame; frame++)
        {
            switch ((int) sprtemp[frame].rotate)
            {
            case -1:            // no rotations were found for that frame at all
                Con_Error("R_InitSprites: No patches found for %s frame %c",
                          spriteName, frame + 'A');
            case 0:         // only the first rotation is needed
                break;

            case 1:         // must have all 8 frames
                for(rotation = 0; rotation < 8; rotation++)
                    if(sprtemp[frame].lump[rotation] == -1)
                        Con_Error("R_InitSprites: Sprite %s frame %c is "
                                  "missing rotations", spriteName,
                                  frame + 'A');
            }
        }

        // The model definitions might have a larger max frame for this
        // sprite.
        /*highframe = R_GetMaxMDefSpriteFrame(spriteName) + 1;
           if(highframe > maxFrame)
           {
           maxFrame = highframe;
           for(l=0; l<maxFrame; l++)
           for(frame=0; frame<8; frame++)
           if(sprtemp[l].lump[frame] == -1)
           sprtemp[l].lump[frame] = 0;
           } */

        // Allocate space for the frames present and copy sprtemp to it.
        sprites[i].numframes = maxFrame;
        sprites[i].spriteframes =
            Z_Malloc(maxFrame * sizeof(spriteframe_t), PU_SPRITE, NULL);
        memcpy(sprites[i].spriteframes, sprtemp,
               maxFrame * sizeof(spriteframe_t));
        // The possible model frames are initialized elsewhere.
        //sprites[i].modeldef = -1;
    }
}

void R_GetSpriteInfo(int sprite, int frame, spriteinfo_t *sprinfo)
{
    spritedef_t *sprdef;
    spriteframe_t *sprframe;
    spritelump_t *sprlump;

#ifdef RANGECHECK
    if((unsigned) sprite >= (unsigned) numSprites)
        Con_Error("R_GetSpriteInfo: invalid sprite number %i.\n", sprite);
#endif

    sprdef = &sprites[sprite];

    if(frame >= sprdef->numframes)
    {
        // We have no information to return.
        memset(sprinfo, 0, sizeof(*sprinfo));
        return;
    }

    sprframe = &sprdef->spriteframes[frame];
    sprlump = spritelumps[sprframe->lump[0]];

    sprinfo->numFrames = sprdef->numframes;
    sprinfo->lump = sprframe->lump[0];
    sprinfo->realLump = sprlump->lump;
    sprinfo->flip = sprframe->flip[0];
    sprinfo->offset = sprlump->offset;
    sprinfo->topOffset = sprlump->topoffset;
    sprinfo->width = sprlump->width;
    sprinfo->height = sprlump->height;
}

void R_GetPatchInfo(int lump, spriteinfo_t *info)
{
    lumppatch_t *patch = (lumppatch_t *) W_CacheLumpNum(lump, PU_CACHE);

    memset(info, 0, sizeof(*info));
    info->lump = info->realLump = lump;
    info->width = SHORT(patch->width);
    info->height = SHORT(patch->height);
    info->topOffset = SHORT(patch->topoffset);
    info->offset = SHORT(patch->leftoffset);
}

/*
 * Returns the radius of the mobj as it would visually appear to be.
 */
int R_VisualRadius(mobj_t *mo)
{
    modeldef_t *mf, *nextmf;
    spriteinfo_t sprinfo;

    // If models are being used, use the model's radius.
    if(useModels)
    {
        R_CheckModelFor(mo, &mf, &nextmf);
        if(mf)
        {
            // Returns the model's radius!
            return mf->visualradius;
        }
    }

    // Use the sprite frame's width.
    R_GetSpriteInfo(mo->sprite, mo->frame, &sprinfo);
    return sprinfo.width / 2;
}

void R_InitSprites(void)
{
    // Free all previous sprite memory.
    Z_FreeTags(PU_SPRITE, PU_SPRITE);
    R_InitSpriteDefs();
    R_InitSpriteLumps();
}

/*
 * Called at frame start.
 */
void R_ClearSprites(void)
{
    vissprite_p = vissprites;
}

vissprite_t *R_NewVisSprite(void)
{
    if(vissprite_p == &vissprites[MAXVISSPRITES])
        return &overflowSprite;
    vissprite_p++;
    return vissprite_p - 1;
}

static void R_ApplyPlaneGlowsToVisSprite(vissprite_t *vis, sector_t *sect)
{
    int     c;

    vis->data.mo.hasglow = false;
    if(!useWallGlow)
        return;

    // Do ceiling.
    if(sect->planes[PLN_CEILING]->glow)
    {
        for(c = 0; c < 3; ++c)
            vis->data.mo.ceilglow[c] =
                sect->planes[PLN_CEILING]->glowrgb[c] * dlFactor;

        vis->data.mo.hasglow = true;
        vis->data.mo.ceilglowamount = sect->planes[PLN_CEILING]->glow;
    }
    else
    {
        for(c = 0; c < 3; ++c)
            vis->data.mo.ceilglow[c] = 0;
    }

    // Do floor.
    if(sect->planes[PLN_FLOOR]->glow)
    {
        for(c = 0; c < 3; ++c)
            vis->data.mo.floorglow[c] =
                sect->planes[PLN_FLOOR]->glowrgb[c] * dlFactor;

        vis->data.mo.hasglow = true;
        vis->data.mo.floorglowamount = sect->planes[PLN_FLOOR]->glow;
    }
    else
    {
        for(c = 0; c < 3; ++c)
            vis->data.mo.floorglow[c] = 0;
    }
}

/*
 * If 3D models are found for psprites, here we will create vissprites
 * for them.
 */
void R_ProjectPlayerSprites(void)
{
    int         i;
    angle_t     ang;
    modeldef_t *mf, *nextmf;
    ddpsprite_t *psp;
    vissprite_t *vis;
    mobj_t      dummy;

    psp3d = false;

    // Cameramen have no psprites.
    if((viewplayer->flags & DDPF_CAMERA) || (viewplayer->flags & DDPF_CHASECAM))
        return;

    for(i = 0, psp = viewplayer->psprites; i < DDMAXPSPRITES; ++i, psp++)
    {
        vis = vispsprites + i;
        psp->flags &= ~DDPSPF_RENDERED;
        vis->type = false;

        vis->distance = 4;
        vis->data.mo.subsector = viewplayer->mo->subsector;
        vis->data.mo.flags = 0;
        // Adjust the vector slightly so an angle can be calculated.
        ang = viewangle >>ANGLETOFINESHIFT;
        vis->center[VX] = FIX2FLT(viewx + (2 * finecosine[ang]));
        vis->center[VY] = FIX2FLT(viewy + (2 * finesine[ang]));
        vis->data.mo.v1[VX] = vis->center[VX];
        vis->data.mo.v1[VY] = vis->center[VY];
        // 32 is the raised weapon height.
        vis->data.mo.gzt = vis->center[VZ] = FIX2FLT(viewz);
        vis->data.mo.secfloor = viewplayer->mo->subsector->sector->SP_floorvisheight;
        vis->data.mo.secceil = viewplayer->mo->subsector->sector->SP_ceilvisheight;
        vis->data.mo.pclass = 0;
        vis->data.mo.floorclip = 0;

        // Glowing floor and ceiling.
        R_ApplyPlaneGlowsToVisSprite(vis, viewplayer->mo->subsector->sector);

        if(!useModels || !psp->stateptr)
            continue;

        // Is there a model for this frame?
        // Setup a dummy for the call to R_CheckModelFor.
        dummy.state = psp->stateptr;
        dummy.tics = psp->tics;

        vis->data.mo.inter = R_CheckModelFor(&dummy, &mf, &nextmf);
        if(!mf)
        {
            // No, draw a 2D sprite instead (in Rend_DrawPlayerSprites).
            continue;
        }
        vis->type = VSPR_HUD_MODEL; // it's a psprite
        vis->light = NULL;
        vis->data.mo.mf = mf;
        vis->data.mo.nextmf = nextmf;
        vis->data.mo.viewaligned = true;
        // Use the exact center with HUD models.
        vis->center[VX] = FIX2FLT(viewx);
        vis->center[VY] = FIX2FLT(viewy);
        vis->data.mo.v1[VX] = vis->center[VX];
        vis->data.mo.v1[VY] = vis->center[VY];

        // Mark this sprite rendered.
        psp->flags |= DDPSPF_RENDERED;

        // There are 3D psprites.
        psp3d = true;

        // Offsets to rotation angles.
        vis->data.mo.v2[VX] = psp->x * weaponOffsetScale - 90;
        vis->data.mo.v2[VY] =
            (32 - psp->y) * weaponOffsetScale * weaponOffsetScaleY / 1000.0f;
        // Is the FOV shift in effect?
        if(weaponFOVShift > 0 && fieldOfView > 90)
            vis->data.mo.v2[VY] -= weaponFOVShift * (fieldOfView - 90) / 90;
        // Real rotation angles.
        vis->data.mo.yaw =
            viewangle / (float) ANGLE_MAX *-360 + vis->data.mo.v2[VX] + 90;
        vis->data.mo.pitch = viewpitch * 85 / 110 + vis->data.mo.v2[VY];
        vis->data.mo.flip = false;

        vis->data.mo.alpha = psp->alpha;

        if(useBias)
        {
            /** Evaluate the position of this player in the light grid.
            * \todo Should be affected by BIAS sources.
	    */
            float       point[3];

            point[0] = FIX2FLT(viewplayer->mo->pos[VX]);
            point[1] = FIX2FLT(viewplayer->mo->pos[VY]);
            point[2] = FIX2FLT(viewplayer->mo->pos[VZ]) +
                            viewplayer->mo->height / 2;
            LG_Evaluate(point, vis->data.mo.rgb);

            vis->data.mo.lightlevel = 1;
        }
        else
        {
            memcpy(vis->data.mo.rgb,
                   R_GetSectorLightColor(viewplayer->mo->subsector->sector),
                   sizeof(float) * 3);

            if(psp->light < 1)
            {
                vis->data.mo.lightlevel = (psp->light - .1f);
                Rend_ApplyLightAdaptation(&vis->data.mo.lightlevel);
            }
            else
                vis->data.mo.lightlevel = 1;
        }

        memset(vis->data.mo.visoff, 0, sizeof(vis->data.mo.visoff));
    }
}

float R_MovementYaw(fixed_t momx, fixed_t momy)
{
    // Multiply by 100 to get some artificial accuracy in bamsAtan2.
    return BANG2DEG(bamsAtan2(-100 * FIX2FLT(momy), 100 * FIX2FLT(momx)));
}

float R_MovementPitch(fixed_t momx, fixed_t momy, fixed_t momz)
{
    return
        BANG2DEG(bamsAtan2
                 (100 * FIX2FLT(momz), 100 * P_AccurateDistance(momx, momy)));
}

/*
 * Determine the correct Z coordinate for the mobj.  The visible Z
 * coordinate may be slightly different than the actual Z coordinate
 * due to smoothed plane movement.
 */
boolean RIT_VisMobjZ(sector_t *sector, void *data)
{
    vissprite_t *vis = data;

    assert(sector != NULL);
    assert(data != NULL);

    if(vis->data.mo.flooradjust &&
       FIX2FLT(projectedThing->pos[VZ]) == sector->SP_floorheight)
    {
        vis->center[VZ] = sector->SP_floorvisheight;
    }

    if(FIX2FLT(projectedThing->pos[VZ]) + projectedThing->height ==
       sector->SP_ceilheight)
    {
        vis->center[VZ] = sector->SP_ceilvisheight - projectedThing->height;
    }
    return true;
}

/**
 * Generates a vissprite for a thing if it might be visible.
 */
void R_ProjectSprite(mobj_t *thing)
{
    sector_t   *sect = thing->subsector->sector;
    fixed_t     trx, try;
    spritedef_t *sprdef;
    spriteframe_t *sprframe = NULL;
    int         i, lump;
    unsigned    rot;
    boolean     flip;
    vissprite_t *vis;
    angle_t     ang;
    float       v1[2], v2[2];
    float       sinrv, cosrv, thangle = 0;  // rv = real value
    boolean     align;
    modeldef_t *mf = NULL, *nextmf = NULL;
    float       interp = 0, distance;

    v2[VX] = v2[VY] = 0; // initialize

    if(thing->ddflags & DDMF_DONTDRAW || thing->translucency == 0xff ||
       thing->state == NULL || thing->state == states)
    {
        // Never make a vissprite when DDMF_DONTDRAW is set or when
        // the thing is fully transparent, or when the thing hasn't got
        // a valid state.
        return;
    }

    // Transform the origin point.
    trx = thing->pos[VX] - viewx;
    try = thing->pos[VY] - viewy;

    // Decide which patch to use for sprite relative to player.

#ifdef RANGECHECK
    if((unsigned) thing->sprite >= (unsigned) numSprites)
    {
        Con_Error("R_ProjectSprite: invalid sprite number %i\n",
                  thing->sprite);
    }
#endif
    sprdef = &sprites[thing->sprite];
    if(thing->frame >= sprdef->numframes)
    {
        // The frame is not defined, we can't display this object.
        return;
    }
    sprframe = &sprdef->spriteframes[thing->frame];

    // Calculate edges of the shape.
    v1[VX] = FIX2FLT(thing->pos[VX]);
    v1[VY] = FIX2FLT(thing->pos[VY]);

    distance = Rend_PointDist2D(v1);

    // Check for a 3D model.
    if(useModels)
    {
        interp = R_CheckModelFor(thing, &mf, &nextmf);
        if(mf && !(mf->flags & MFF_NO_DISTANCE_CHECK) && maxModelDistance &&
           distance > maxModelDistance)
        {
            // Don't use a 3D model.
            mf = nextmf = NULL;
            interp = -1;
        }
    }

    if(sprframe->rotate && !mf)
    {
        // Choose a different rotation based on player view.
        ang = R_PointToAngle(thing->pos[VX], thing->pos[VY]);
        rot = (ang - thing->angle + (unsigned) (ANG45 / 2) * 9) >> 29;
        lump = sprframe->lump[rot];
        flip = (boolean) sprframe->flip[rot];
    }
    else
    {
        // Use single rotation for all views.
        lump = sprframe->lump[0];
        flip = (boolean) sprframe->flip[0];
    }
    // Align to the view plane?
    if(thing->ddflags & DDMF_VIEWALIGN)
        align = true;
    else
        align = false;

    if(alwaysAlign == 1)
        align = true;

    if(!mf)
    {
        if(align || alwaysAlign == 3)
        {
            // The sprite should be fully aligned to view plane.
            sinrv = -FIX2FLT(viewcos);
            cosrv = FIX2FLT(viewsin);
        }
        else
        {
            thangle =
                BANG2RAD(bamsAtan2(FIX2FLT(try) * 10, FIX2FLT(trx) * 10)) -
                PI / 2;
            sinrv = sin(thangle);
            cosrv = cos(thangle);
        }

        //if(alwaysAlign == 2) align = true;

        v1[VX] -= cosrv * spritelumps[lump]->offset;
        v1[VY] -= sinrv * spritelumps[lump]->offset;
        v2[VX] = v1[VX] + cosrv * spritelumps[lump]->width;
        v2[VY] = v1[VY] + sinrv * spritelumps[lump]->width;

        if(!align && alwaysAlign != 2 && alwaysAlign != 3)
            // Check for visibility.
            if(!C_CheckViewRelSeg(v1[VX], v1[VY], v2[VX], v2[VY]))
                return;         // Isn't visible.
    }
    else
    {
        // Models need to be visibility-checked, too.
        float   off[2];

        thangle =
            BANG2RAD(bamsAtan2(FIX2FLT(try) * 10, FIX2FLT(trx) * 10)) - PI / 2;
        sinrv = sin(thangle);
        cosrv = cos(thangle);
        off[VX] = cosrv * (thing->radius >> FRACBITS);
        off[VY] = sinrv * (thing->radius >> FRACBITS);
        if(!C_CheckViewRelSeg
           (v1[VX] - off[VX], v1[VY] - off[VY], v1[VX] + off[VX],
            v1[VY] + off[VY]))
        {
            // The visibility check indicates that the model's origin is
            // not visible. However, if the model is close to the viewpoint
            // we will need to draw it. Otherwise large models are likely
            // to disappear too early.
            if(P_ApproxDistance
               (distance * FRACUNIT,
                thing->pos[VZ] + FLT2FIX(thing->height / 2) - viewz) >
               MAX_OBJECT_RADIUS * FRACUNIT)
            {
                return;         // Can't be visible.
            }
        }
        // Viewaligning means scaling down Z with models.
        align = false;
    }
    //
    // Store information in a vissprite.
    //
    vis = R_NewVisSprite();
    vis->type = VSPR_MAP_OBJECT;
    vis->distance = distance;
    vis->data.mo.subsector = thing->subsector;
    if(thing->usingBias && useBias)
        vis->light = NULL;
    else
        vis->light = DL_GetLuminous(thing->light);

    vis->data.mo.mf = mf;
    vis->data.mo.nextmf = nextmf;
    vis->data.mo.inter = interp;
    vis->data.mo.flags = thing->ddflags;
    vis->data.mo.id = thing->thinker.id;
    vis->data.mo.selector = thing->selector;
    vis->center[VX] = FIX2FLT(thing->pos[VX]);
    vis->center[VY] = FIX2FLT(thing->pos[VY]);
    vis->center[VZ] = FIX2FLT(thing->pos[VZ]);

    vis->data.mo.flooradjust =
        (fabs(sect->SP_floorvisheight - sect->SP_floorheight) < 8);

    // The thing's Z coordinate must match the actual visible
    // floor/ceiling height.  When using smoothing, this requires
    // iterating through the sectors (planes) in the vicinity.
    validcount++;
    projectedThing = thing;
    P_ThingSectorsIterator(thing, RIT_VisMobjZ, vis);

    vis->data.mo.gzt =
        vis->center[VZ] + ((float) spritelumps[lump]->topoffset);

    if(useBias)
    {
        float point[3];

        point[0] = FIX2FLT(thing->pos[VX]);
        point[1] = FIX2FLT(thing->pos[VY]);
        point[2] = FIX2FLT(thing->pos[VZ]) + thing->height / 2;
        LG_Evaluate(point, vis->data.mo.rgb);
    }
    else
    {
        memcpy(vis->data.mo.rgb, R_GetSectorLightColor(sect), sizeof(float) * 3);
    }

    vis->data.mo.viewaligned = align;

    vis->data.mo.secfloor = thing->subsector->sector->SP_floorvisheight;
    vis->data.mo.secceil  = thing->subsector->sector->SP_ceilvisheight;

    if(thing->ddflags & DDMF_TRANSLATION)
        vis->data.mo.pclass = (thing->ddflags >> DDMF_CLASSTRSHIFT) & 0x3;
    else
        vis->data.mo.pclass = 0;

    // Foot clipping.
    vis->data.mo.floorclip = thing->floorclip;
    if(thing->ddflags & DDMF_BOB)
    {
        // Bobbing is applied to the floorclip.
        vis->data.mo.floorclip += R_GetBobOffset(thing);
    }

    // The start and end vertices.
    vis->data.mo.v1[VX] = v1[VX];
    vis->data.mo.v1[VY] = v1[VY];
    if(!mf)
    {
        vis->data.mo.v2[VX] = v2[VX];
        vis->data.mo.v2[VY] = v2[VY];
    }
    else
    {
        // Determine the rotation angles (in degrees).
        if(mf->sub[0].flags & MFF_ALIGN_YAW)
        {
            vis->data.mo.yaw = 90 - thangle / PI * 180;
        }
        else if(mf->sub[0].flags & MFF_SPIN)
        {
            vis->data.mo.yaw = modelSpinSpeed * 70 * levelTime + (long) thing % 360;
        }
        else if(mf->sub[0].flags & MFF_MOVEMENT_YAW)
        {
            vis->data.mo.yaw = R_MovementYaw(thing->mom[MX], thing->mom[MY]);
        }
        else
        {
            vis->data.mo.yaw = (useSRVOAngle && !netgame &&
                                !playback ? (thing->visangle << 16) : thing->
                                angle) / (float) ANGLE_MAX *-360;
        }

        // How about a unique offset?
        if(mf->sub[0].flags & MFF_IDANGLE)
        {
            // Multiply with an arbitrary factor.
            vis->data.mo.yaw += THING_TO_ID(thing) % 360;
        }

        if(mf->sub[0].flags & MFF_ALIGN_PITCH)
        {
            vis->data.mo.pitch =
                -BANG2DEG(bamsAtan2
                          (((vis->center[VZ] + vis->data.mo.gzt) / 2 -
                            FIX2FLT(viewz)) * 10, distance * 10));
        }
        else if(mf->sub[0].flags & MFF_MOVEMENT_PITCH)
        {
            vis->data.mo.pitch =
                R_MovementPitch(thing->mom[MX], thing->mom[MY], thing->mom[MZ]);
        }
        else
            vis->data.mo.pitch = 0;
    }
    vis->data.mo.flip = flip;
    vis->data.mo.patch = lump;

    // Set light level.
    if((levelFullBright || thing->state->flags & STF_FULLBRIGHT) &&
       (!mf || !(mf->sub[0].flags & MFF_DIM)))
    {
        vis->data.mo.lightlevel = -1; // fullbright
    }
    else if(useBias)
    {
        // The light color has been evaluated with the light grid.
        vis->data.mo.lightlevel = 1;
    }
    else
    {
        // Diminished light (with compression).
        vis->data.mo.lightlevel = sect->lightlevel;
        Rend_ApplyLightAdaptation(&vis->data.mo.lightlevel);
    }

    // The three highest bits of the selector are used for an alpha level.
    // 0 = opaque (alpha -1)
    // 1 = 1/8 transparent
    // 4 = 1/2 transparent
    // 7 = 7/8 transparent
    i = thing->selector >> DDMOBJ_SELECTOR_SHIFT;
    if(i & 0xe0)
    {
        vis->data.mo.alpha = 1 - ((i & 0xe0) >> 5) / 8.0f;
    }
    else
    {
        if(thing->translucency)
            vis->data.mo.alpha = 1 - thing->translucency * reciprocal255;
        else
            vis->data.mo.alpha = -1;
    }

    // Reset the visual offset.
    memset(vis->data.mo.visoff, 0, sizeof(vis->data.mo.visoff));

    // Short-range visual offsets.
    if((vis->data.mo.mf && useSRVO > 0) ||
       (!vis->data.mo.mf && useSRVO > 1))
    {
        if(thing->state && thing->tics >= 0)
        {
            float   mul =
                (thing->tics - frameTimePos) / (float) thing->state->tics;
            for(i = 0; i < 3; i++)
                vis->data.mo.visoff[i] = FIX2FLT(thing->srvo[i] << 8) * mul;
        }
        if(thing->mom[MX] || thing->mom[MY] || thing->mom[MZ])
        {
            // Use the object's speed to calculate a short-range
            // offset.
            vis->data.mo.visoff[VX] += FIX2FLT(thing->mom[MX]) * frameTimePos;
            vis->data.mo.visoff[VY] += FIX2FLT(thing->mom[MY]) * frameTimePos;
            vis->data.mo.visoff[VZ] += FIX2FLT(thing->mom[MZ]) * frameTimePos;
        }
    }

    // Glowing floor and ceiling.
    R_ApplyPlaneGlowsToVisSprite(vis, sect);
}

void R_AddSprites(sector_t *sec)
{
    float       visibleTop;
    mobj_t      *thing;
    spriteinfo_t spriteInfo;
    boolean     raised = false;

    // Don't use validcount, because other parts of the renderer may
    // change it.
    if(sec->addspritecount == framecount)
        return;                 // already added

    sec->addspritecount = framecount;

    for(thing = sec->thinglist; thing; thing = thing->snext)
    {
        R_ProjectSprite(thing);

        // Hack: Sprites have a tendency to extend into the ceiling in
        // sky sectors. Here we will raise the skyfix dynamically, at
        // runtime, to make sure that no sprites get clipped by the sky.
        // Only check
        if(R_IsSkySurface(&sec->SP_ceilsurface))
        {
            if(!(thing->dplayer && thing->dplayer->flags & DDPF_CAMERA) && // Cameramen don't exist!
               thing->pos[VZ] <= sec->SP_ceilheight && 
               thing->pos[VZ] >= sec->SP_floorheight && !sec->selfRefHack)
            {
                R_GetSpriteInfo(thing->sprite, thing->frame, &spriteInfo);
                visibleTop =
                    FIX2FLT(thing->pos[VZ] + (spriteInfo.height << FRACBITS));

                if(visibleTop >
                    sec->SP_ceilheight + sec->skyfix[PLN_CEILING].offset)
                {
                    // Raise sector skyfix.
                    sec->skyfix[PLN_CEILING].offset =
                        visibleTop - sec->SP_ceilheight + 16; // Add some leeway.
                    raised = true;
                }
            }
        }
    }

    // This'll adjust all adjacent sky ceiling fixes.
    if(raised)
        R_SkyFix(false, true);
}

vissprite_t vsprsortedhead;

void R_SortVisSprites(void)
{
    int     i, count;
    vissprite_t *ds, *best = 0;
    vissprite_t unsorted;
    float   bestdist;

    count = vissprite_p - vissprites;

    unsorted.next = unsorted.prev = &unsorted;
    if(!count)
        return;

    for(ds = vissprites; ds < vissprite_p; ds++)
    {
        ds->next = ds + 1;
        ds->prev = ds - 1;
    }
    vissprites[0].prev = &unsorted;
    unsorted.next = &vissprites[0];
    (vissprite_p - 1)->next = &unsorted;
    unsorted.prev = vissprite_p - 1;

    //
    // Pull the vissprites out by distance.
    //
    vsprsortedhead.next = vsprsortedhead.prev = &vsprsortedhead;
    for(i = 0; i < count; i++)
    {
        bestdist = 0;
        for(ds = unsorted.next; ds != &unsorted; ds = ds->next)
        {
            if(ds->distance >= bestdist)
            {
                bestdist = ds->distance;
                best = ds;
            }
        }
        best->next->prev = best->prev;
        best->prev->next = best->next;
        best->next = &vsprsortedhead;
        best->prev = vsprsortedhead.prev;
        vsprsortedhead.prev->next = best;
        vsprsortedhead.prev = best;
    }
}

/*
 * Returns the current floatbob offset for the mobj, if the mobj is flagged
 * for bobbing.
 */
float R_GetBobOffset(mobj_t *mo)
{
    if(mo->ddflags & DDMF_BOB)
    {
        return (sin(THING_TO_ID(mo) + levelTime / 1.8286 * 2 * PI) * 8);
    }
    return 0;
}
