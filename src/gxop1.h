/* Copyright (C) 1991, 1992, 1998 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gxop1.h,v 1.2 2004/02/14 22:20:18 atai Exp $ */
/* Type 1 state shared between interpreter and compiled fonts. */

#ifndef gxop1_INCLUDED
#  define gxop1_INCLUDED

/*
 * The current point (px,py) in the Type 1 interpreter state is not
 * necessarily the same as the current position in the path being built up.
 * Specifically, (px,py) may not reflect adjustments for hinting,
 * whereas the current path position does reflect those adjustments.
 */

/* Define the shared Type 1 interpreter state. */
#define max_coeff_bits 11	/* max coefficient in char space */
typedef struct gs_op1_state_s {
    struct gx_path_s *ppath;
    struct gs_type1_state_s *pcis;
    fixed_coeff fc;
    gs_fixed_point co;		/* character origin (device space) */
    gs_fixed_point p;		/* current point (device space) */
} gs_op1_state;
typedef gs_op1_state *is_ptr;

/* Define the state used by operator procedures. */
/* These macros refer to a current instance (s) of gs_op1_state. */
#define sppath s.ppath
#define sfc s.fc
#define spt s.p
#define ptx s.p.x
#define pty s.p.y

/* Accumulate relative coordinates */
/****** THESE ARE NOT ACCURATE FOR NON-INTEGER DELTAS. ******/
/* This probably doesn't make any difference in practice. */
#define c_fixed(d, c) m_fixed(d, c, sfc, max_coeff_bits)
#define accum_x(dx)\
  BEGIN\
    ptx += c_fixed(dx, xx);\
    if ( sfc.skewed ) pty += c_fixed(dx, xy);\
  END
#define accum_y(dy)\
  BEGIN\
    pty += c_fixed(dy, yy);\
    if ( sfc.skewed ) ptx += c_fixed(dy, yx);\
  END
void accum_xy_proc(is_ptr ps, fixed dx, fixed dy);

#define accum_xy(dx,dy)\
  accum_xy_proc(&s, dx, dy)

/* Define operator procedures. */
int gs_op1_closepath(is_ptr ps);
int gs_op1_rrcurveto(is_ptr ps, fixed dx1, fixed dy1,
		     fixed dx2, fixed dy2, fixed dx3, fixed dy3);

#endif /* gxop1_INCLUDED */