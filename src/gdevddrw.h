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

/* $Id: gdevddrw.h,v 1.2 2005/04/18 12:05:56 Arabidopsis Exp $ */
/* Prototypes of some polygon and image drawing procedures */

#ifndef gdevddrw_INCLUDED
#  define gdevddrw_INCLUDED

enum fill_trap_flags {
    ftf_peak0 = 1,
    ftf_peak1 = 2,
    ftf_pseudo_rasterization = 4
};

int
gx_fill_trapezoid_narrow(gx_device * dev, const gs_fixed_edge * left,
    const gs_fixed_edge * right, fixed ybot, fixed ytop, int flags,
    const gx_device_color * pdevc, gs_logical_operation_t lop);

#endif /* gdevddrw_INCLUDED */
