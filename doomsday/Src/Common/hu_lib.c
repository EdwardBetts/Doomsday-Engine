// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
//
// DESCRIPTION:  heads-up text and input code
//
//		Compiles for jDoom, jHeretic, jHexen
//-----------------------------------------------------------------------------

// HEADER FILES ------------------------------------------------------------

#include <ctype.h>

#ifdef __JDOOM__
# include "jDoom/doomdef.h"
# include "jDoom/d_config.h"
# include "jDoom/r_local.h"
#elif __JHERETIC__
# include "jHeretic/Doomdef.h"
# include "jHeretic/h_config.h"
# include "jHeretic/R_local.h"
#elif __JHEXEN__
# include "jHexen/h2def.h"
# include "jHexen/x_config.h"
# include "jHexen/r_local.h"
#elif __JSTRIFE__
# include "jStrife/h2def.h"
# include "jStrife/d_config.h"
# include "jStrife/r_local.h"
#endif

#include "Common/m_swap.h"
#include "Common/hu_lib.h"
#include "r_draw.h"

// MACROS ------------------------------------------------------------------

// boolean : whether the screen is always erased
#define noterased viewwindowx

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

extern boolean automapactive;	// in AM_map.c

// PUBLIC DATA DEFINITIONS -------------------------------------------------

// PRIVATE DATA DEFINITIONS ------------------------------------------------

// CODE --------------------------------------------------------------------

/*
 *  HUlib_init
 */
void HUlib_init(void)
{
	// Nothing to do...
}

/*
 *  HUlib_clearTextLine
 */
void HUlib_clearTextLine(hu_textline_t * t)
{
	t->len = 0;
	t->l[0] = 0;
	t->needsupdate = true;
}

/*
 *  HUlib_initTextLine
 */
void HUlib_initTextLine(hu_textline_t * t, int x, int y, dpatch_t * f, int sc)
{
	t->x = x;
	t->y = y;
	t->f = f;
	t->sc = sc;
	HUlib_clearTextLine(t);
}

/*
 *  HUlib_addCharToTextLine
 */
boolean HUlib_addCharToTextLine(hu_textline_t * t, char ch)
{
	if(t->len == HU_MAXLINELENGTH)
		return false;

	t->l[t->len++] = ch;
	t->l[t->len] = 0;
	t->needsupdate = 4;
	return true;
}

/*
 *  HUlib_delCharFromTextLine
 */
boolean HUlib_delCharFromTextLine(hu_textline_t * t)
{
	if(!t->len)
		return false;

	t->l[--t->len] = 0;
	t->needsupdate = 4;
	return true;
}

/*
 *  HUlib_drawTextLine
 */
void HUlib_drawTextLine(hu_textline_t * l, boolean drawcursor)
{
	int     i;
	int     w;
	int     x;
	unsigned char c;

	gl.Color3fv(cfg.hudColor);

	// draw the new stuff
	x = l->x;
	for(i = 0; i < l->len; i++)
	{
		c = toupper(l->l[i]);
		if(c != ' ' && c >= l->sc && c <= '_')
		{
			w = SHORT(l->f[c - l->sc].width);
			if(x + w > SCREENWIDTH)
				break;
			GL_DrawPatch_CS(x, l->y, l->f[c - l->sc].lump);
			x += w;
		}
		else
		{
			x += 4;
			if(x >= SCREENWIDTH)
				break;
		}
	}

	// draw the cursor if requested
	if(drawcursor && x + SHORT(l->f['_' - l->sc].width) <= SCREENWIDTH)
		GL_DrawPatch_CS(x, l->y, l->f['_' - l->sc].lump);
}

/*
 *  HUlib_eraseTextLine
 *		Sorta called by HU_Erase and just better darn get things straight
 */
void HUlib_eraseTextLine(hu_textline_t * l)
{
	if(l->needsupdate)
		l->needsupdate--;
}

/*
 *  HUlib_initSText
 */
void HUlib_initSText(hu_stext_t * s, int x, int y, int h, dpatch_t * font,
					 int startchar, boolean *on)
{
	int     i;

	s->h = h;
	s->on = on;
	s->laston = true;
	s->cl = 0;
	for(i = 0; i < h; i++)
		HUlib_initTextLine(&s->l[i], x, y - i * (SHORT(font[0].height) + 1),
						   font, startchar);
}

/*
 *  HUlib_addLineToSText
 */
void HUlib_addLineToSText(hu_stext_t * s)
{
	int     i;

	// add a clear line
	if(++s->cl == s->h)
		s->cl = 0;

	HUlib_clearTextLine(&s->l[s->cl]);

	// everything needs updating
	for(i = 0; i < s->h; i++)
		s->l[i].needsupdate = 4;
}

/*
 *  HUlib_addMessageToSText
 */
void HUlib_addMessageToSText(hu_stext_t * s, char *prefix, char *msg)
{
	HUlib_addLineToSText(s);
	if(prefix)
		while(*prefix)
			HUlib_addCharToTextLine(&s->l[s->cl], *(prefix++));

	while(*msg)
		HUlib_addCharToTextLine(&s->l[s->cl], *(msg++));
}

/*
 *  HUlib_drawSText
 */
void HUlib_drawSText(hu_stext_t * s)
{
	int     i, idx;
	hu_textline_t *l;

	if(!*s->on)
		return;					// if not on, don't draw

	// draw everything
	for(i = 0; i < s->h; i++)
	{
		idx = s->cl - i;
		if(idx < 0)
			idx += s->h;		// handle queue of lines

		l = &s->l[idx];

		// need a decision made here on whether to skip the draw
		HUlib_drawTextLine(l, false);	// no cursor, please
	}

}

/*
 *  HUlib_eraseSText
 */
void HUlib_eraseSText(hu_stext_t * s)
{

	int     i;

	for(i = 0; i < s->h; i++)
	{
		if(s->laston && !*s->on)
			s->l[i].needsupdate = 4;
		HUlib_eraseTextLine(&s->l[i]);
	}
	s->laston = *s->on;

}

/*
 *  HUlib_initIText
 */
void HUlib_initIText(hu_itext_t * it, int x, int y, dpatch_t * font,
					 int startchar, boolean *on)
{
	it->lm = 0;					// default left margin is start of text
	it->on = on;
	it->laston = true;
	HUlib_initTextLine(&it->l, x, y, font, startchar);
}

/*
 *  HUlib_delCharFromIText
 *		Adheres to the left margin restriction
 */
void HUlib_delCharFromIText(hu_itext_t * it)
{
	if(it->l.len != it->lm)
		HUlib_delCharFromTextLine(&it->l);
}

/*
 *  HUlib_eraseLineFromIText
 */
void HUlib_eraseLineFromIText(hu_itext_t * it)
{
	while(it->lm != it->l.len)
		HUlib_delCharFromTextLine(&it->l);
}

/*
 *  HUlib_resetIText
 *		Resets left margin as well
 */
void HUlib_resetIText(hu_itext_t * it)
{
	it->lm = 0;
	HUlib_clearTextLine(&it->l);
}

/*
 *  HUlib_addPrefixToIText
 */
void HUlib_addPrefixToIText(hu_itext_t * it, char *str)
{
	while(*str)
		HUlib_addCharToTextLine(&it->l, *(str++));
	it->lm = it->l.len;
}

/*
 *   HUlib_keyInIText
 *		Wrapper function for handling general keyed input.
 *		returns true if it ate the key
 */
boolean HUlib_keyInIText(hu_itext_t * it, unsigned char ch)
{
	if(ch >= ' ' && ch <= '_')
		HUlib_addCharToTextLine(&it->l, (char) ch);
	else if(ch == DDKEY_BACKSPACE)
		HUlib_delCharFromIText(it);
	else if(ch != DDKEY_ENTER)
		return false;			// did not eat key

	return true;				// ate the key
}

/*
 *  HUlib_drawIText
 */
void HUlib_drawIText(hu_itext_t * it)
{
	hu_textline_t *l = &it->l;

	if(!*it->on)
		return;
	HUlib_drawTextLine(l, true);	// draw the line w/ cursor
}

/*
 *  HUlib_eraseIText
 */
void HUlib_eraseIText(hu_itext_t * it)
{
	if(it->laston && !*it->on)
		it->l.needsupdate = 4;
	HUlib_eraseTextLine(&it->l);
	it->laston = *it->on;
}
