/* Copyright (C) 2002 artofcode LLC. All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License version 2
  as published by the Free Software Foundation.


  This software is provided AS-IS with no warranty, either express or
  implied. That is, this program is distributed in the hope that it will 
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place, Suite 330, Boston, MA, 02111-1307.
   
  For more information about licensing, please refer to
  http://www.ghostscript.com/licensing/. For information on
  commercial licensing, go to http://www.artifex.com/licensing/ or
  contact Artifex Software, Inc., 101 Lucas Valley Road #110,
  San Rafael, CA  94903, U.S.A., +1(415)492-9861.  
*/

/* $Id: gxoprect.h,v 1.2 2005/04/18 12:06:00 Arabidopsis Exp $ */
/* geneic overprint fill rectangle interface */

#ifndef gxoprect_INCLUDED
#define gxoprect_INCLUDED

/*
 * Perform the fill rectangle operation for a non-separable color encoding
 * that requires overprint support.
 *
 * Returns 0 on success, < 0 in the event of an error.
 */
extern  int     gx_overprint_generic_fill_rectangle(
    gx_device *             tdev,
    gx_color_index          drawn_comps,
    int                     x,
    int                     y,
    int                     w,
    int                     h,
    gx_color_index          color,
    gs_memory_t *           mem );

/*
 * Perform the fill rectangle operation of a separable color encoding.
 * There are two versions of this routine: ..._1 for cases in which the
 * color depth is a divisor of 8 * sizeof(mono_fill_chunk), and ..._2 if
 * this is not the case (most typically if the depth == 24).
 *
 * For both cases, the color and retain_mask values passed to this
 * procedure are expected to be already swapped as required for a byte-
 * oriented bitmap. This consideration affects only little-endian
 * machines. For those machines, if depth > 9 the color passed to these
 * two procedures will not be the same as that passed to
 * gx_overprint_generic_fill_rectangle.
 *
 * Returns 0 on success, < 0 in the event of an error.
 */
extern  int     gx_overprint_sep_fill_rectangle_1(
    gx_device *             tdev,
    gx_color_index          retain_mask,    /* already swapped */
    int                     x,
    int                     y,
    int                     w,
    int                     h,
    gx_color_index          color,          /* already swapped */
    gs_memory_t *           mem );

extern  int     gx_overprint_sep_fill_rectangle_2(
    gx_device *             tdev,
    gx_color_index          retain_mask,    /* already swapped */
    int                     x,
    int                     y,
    int                     w,
    int                     h,
    gx_color_index          color,          /* already swapped */
    gs_memory_t *           mem );

#endif  /* gxoprect_INCLUDED */
