/* Copyright (C) 1995, 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gxctable.h,v 1.2 2004/02/14 22:20:18 atai Exp $ */
/* Interface to color table lookup and interpolation */

#ifndef gxctable_INCLUDED
#  define gxctable_INCLUDED

#include "gxfixed.h"
#include "gxfrac.h"

/*
 * Define a 3- or 4-D color lookup table.
 * n is the number of dimensions (input indices), 3 or 4.
 * dims[0..n-1] are the table dimensions.
 * m is the number of output values, typically 3 (RGB) or 4 (CMYK).
 * For n = 3:
 *   table[i], 0 <= i < dims[0], point to strings of length
 *     dims[1] x dims[2] x m.
 * For n = 4:
 *   table[i], 0 <= i < dims[0] x dims[1], points to strings of length
 *     dims[2] x dims[3] x m.
 * It isn't really necessary to store the size of each string, since
 * they're all the same size, but it makes things a lot easier for the GC.
 */
typedef struct gx_color_lookup_table_s {
    int n;
    int dims[4];		/* [ndims] */
    int m;
    const gs_const_string *table;
} gx_color_lookup_table;

/*
 * Interpolate in a 3- or 4-D color lookup table.
 * pi[0..n-1] are the table indices, guaranteed to be in the ranges
 * [0..dims[n]-1] respectively.
 * Return interpolated values in pv[0..m-1].
 */

/* Return the nearest value without interpolation. */
void gx_color_interpolate_nearest(const fixed * pi,
			    const gx_color_lookup_table * pclt, frac * pv);

/* Use trilinear interpolation. */
void gx_color_interpolate_linear(const fixed * pi,
			    const gx_color_lookup_table * pclt, frac * pv);

#endif /* gxctable_INCLUDED */