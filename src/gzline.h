/* Copyright (C) 1989, 1995, 1996 Aladdin Enterprises.  All rights reserved.
  
  This file is part of GNU ghostscript

  GNU ghostscript is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2, or (at your option) any later version.

  GNU ghostscript is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  ghostscript; see the file COPYING. If not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

  
*/

/* $Id: gzline.h,v 1.4 2005/12/13 16:57:25 jemarch Exp $ */
/* Line parameter implementation */

#ifndef gzline_INCLUDED
#  define gzline_INCLUDED

#include "gxline.h"

/*
 * The GC procedures are complex because we must not attempt to follow
 * the pattern pointer iff the pattern size is 0.
 */
#define private_st_line_params() /* in gsistate.c */\
  gs_private_st_complex_only(st_line_params, gx_line_params, "line_params",\
    0, line_params_enum_ptrs, line_params_reloc_ptrs, 0)
#define st_line_params_num_ptrs 1

/* Internal accessor for line parameters in graphics state */
const gx_line_params *gs_currentlineparams(const gs_imager_state *);

#endif /* gzline_INCLUDED */
