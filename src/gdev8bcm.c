/* Copyright (C) 1994 artofcode LLC.  All rights reserved.
  
  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place, Suite 330, Boston, MA, 02111-1307.

*/

/*$Id: gdev8bcm.c,v 1.1 2004/01/14 16:59:47 atai Exp $ */
/* Dynamic color mapping for 8-bit displays */
#include "gx.h"
#include "gxdevice.h"
#include "gdev8bcm.h"

/* Initialize an 8-bit color map. */
void
gx_8bit_map_init(gx_8bit_color_map * pcm, int max_count)
{
    int i;

    pcm->count = 0;
    pcm->max_count = max_count;
    for (i = 0; i < gx_8bit_map_size; i++)
	pcm->map[i].rgb = gx_8bit_no_rgb;
}

/* Look up a color in an 8-bit color map. */
/* Return <0 if not found. */
int
gx_8bit_map_rgb_color(const gx_8bit_color_map * pcm, gx_color_value r,
		      gx_color_value g, gx_color_value b)
{
    ushort rgb = gx_8bit_rgb_key(r, g, b);
    const gx_8bit_map_entry *pme =
    &pcm->map[(rgb * gx_8bit_map_spreader) % gx_8bit_map_size];

    for (;; pme++) {
	if (pme->rgb == rgb)
	    return pme->index;
	else if (pme->rgb == gx_8bit_no_rgb)
	    break;
    }
    if (pme != &pcm->map[gx_8bit_map_size])
	return pme - &pcm->map[gx_8bit_map_size];
    /* We ran off the end; wrap around and continue. */
    pme = &pcm->map[0];
    for (;; pme++) {
	if (pme->rgb == rgb)
	    return pme->index;
	else if (pme->rgb == gx_8bit_no_rgb)
	    return pme - &pcm->map[gx_8bit_map_size];
    }
}

/* Add a color to an 8-bit color map after an unsuccessful lookup, */
/* and return its index.  Return <0 if the map is full. */
int
gx_8bit_add_rgb_color(gx_8bit_color_map * pcm, gx_color_value r,
		      gx_color_value g, gx_color_value b)
{
    int index;
    gx_8bit_map_entry *pme;

    if (gx_8bit_map_is_full(pcm))
	return -1;
    index = gx_8bit_map_rgb_color(pcm, r, g, b);
    if (index >= 0)		/* shouldn't happen */
	return index;
    pme = &pcm->map[-index];
    pme->rgb = gx_8bit_rgb_key(r, g, b);
    return (pme->index = pcm->count++);
}
