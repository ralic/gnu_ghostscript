/* Copyright (C) 1989, 1995, 1996, 1997, 1998, 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gspath.h,v 1.2 2004/02/14 22:20:17 atai Exp $ */
/* Graphics state path procedures */
/* Requires gsstate.h */

#ifndef gspath_INCLUDED
#  define gspath_INCLUDED

#include "gspenum.h"

/* Path constructors */
int gs_newpath(gs_state *),
    gs_moveto(gs_state *, floatp, floatp),
    gs_rmoveto(gs_state *, floatp, floatp),
    gs_lineto(gs_state *, floatp, floatp),
    gs_rlineto(gs_state *, floatp, floatp),
    gs_arc(gs_state *, floatp, floatp, floatp, floatp, floatp),
    gs_arcn(gs_state *, floatp, floatp, floatp, floatp, floatp),
    /*
     * Because of an obscure bug in the IBM RS/6000 compiler, one (but not
     * both) bool argument(s) for gs_arc_add must come before the floatp
     * arguments.
     */
    gs_arc_add(gs_state *, bool, floatp, floatp, floatp, floatp, floatp, bool),
    gs_arcto(gs_state *, floatp, floatp, floatp, floatp, floatp, float[4]),
    gs_curveto(gs_state *, floatp, floatp, floatp, floatp, floatp, floatp),
    gs_rcurveto(gs_state *, floatp, floatp, floatp, floatp, floatp, floatp),
    gs_closepath(gs_state *);

/* Imager-level procedures */
#ifndef gs_imager_state_DEFINED
#  define gs_imager_state_DEFINED
typedef struct gs_imager_state_s gs_imager_state;
#endif
#ifndef gx_path_DEFINED
#  define gx_path_DEFINED
typedef struct gx_path_s gx_path;
#endif
int gs_imager_arc_add(gx_path * ppath, gs_imager_state * pis,
		      bool clockwise, floatp axc, floatp ayc,
		      floatp arad, floatp aang1, floatp aang2,
		      bool add_line);

#define gs_arc_add_inline(pgs, cw, axc, ayc, arad, aa1, aa2, add)\
  gs_imager_arc_add((pgs)->path, (gs_imager_state *)(pgs),\
		    cw, axc, ayc, arad, aa1, aa2, add)

/* Add the current path to the path in the previous graphics state. */
int gs_upmergepath(gs_state *);

/* Path accessors and transformers */
int gs_currentpoint(gs_state *, gs_point *),
      gs_upathbbox(gs_state *, gs_rect *, bool),
      gs_dashpath(gs_state *),
      gs_flattenpath(gs_state *),
      gs_reversepath(gs_state *),
      gs_strokepath(gs_state *);

/* The extra argument for gs_upathbbox controls whether to include */
/* a trailing moveto in the bounding box. */
#define gs_pathbbox(pgs, prect)\
  gs_upathbbox(pgs, prect, false)

/* Path enumeration */

/* This interface conditionally makes a copy of the path. */
gs_path_enum *gs_path_enum_alloc(gs_memory_t *, client_name_t);
int gs_path_enum_copy_init(gs_path_enum *, const gs_state *, bool);

#define gs_path_enum_init(penum, pgs)\
  gs_path_enum_copy_init(penum, pgs, true)
int gs_path_enum_next(gs_path_enum *, gs_point[3]);  /* 0 when done */
void gs_path_enum_cleanup(gs_path_enum *);

/* Clipping */
int gs_clippath(gs_state *),
    gs_initclip(gs_state *),
    gs_clip(gs_state *),
    gs_eoclip(gs_state *);

#endif /* gspath_INCLUDED */