/* Copyright (C) 1994, 1999 artofcode LLC.  All rights reserved.
  
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

/*$Id: icsmap.h,v 1.1 2004/01/14 16:59:52 atai Exp $ */
/* Interface to shared routines for loading the cached color space maps. */

#ifndef icsmap_INCLUDED
#  define icsmap_INCLUDED

/*
 * Set up to load a cached map for an Indexed or substituted Separation
 * color space.  The implementation is in zcsindex.c.  When the map1
 * procedure is called, the following structure is on the e_stack:
 */
#define num_csme 5
#  define csme_num_components (-4)	/* t_integer */
#  define csme_map (-3)		/* t_struct (bytes) */
#  define csme_proc (-2)	/* -procedure- */
#  define csme_hival (-1)	/* t_integer */
#  define csme_index 0		/* t_integer */
/*
 * Note that the underlying color space parameter is a direct space, not a
 * base space, since the underlying space of an Indexed color space may be
 * a Separation or DeviceN space.
 */
int zcs_begin_map(P6(i_ctx_t *i_ctx_p, gs_indexed_map ** pmap,
		     const ref * pproc, int num_entries,
		     const gs_direct_color_space * base_space,
		     op_proc_t map1));

#endif /* icsmap_INCLUDED */
