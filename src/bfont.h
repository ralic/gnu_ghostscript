/* Copyright (C) 1992, 1995, 1996, 1998, 1999 artofcode LLC.  All rights reserved.
  
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

/*$Id: bfont.h,v 1.1 2004/01/14 16:59:47 atai Exp $ */
/* Interpreter internal routines and data needed for building fonts */
/* Requires gxfont.h */

#ifndef bfont_INCLUDED
#  define bfont_INCLUDED

#include "ifont.h"

/* In zfont.c */
int add_FID(P4(i_ctx_t *i_ctx_p, ref *pfdict, gs_font *pfont,
	       gs_ref_memory_t *imem));

font_proc_make_font(zdefault_make_font);
font_proc_make_font(zbase_make_font);
/* The global font directory */
extern gs_font_dir *ifont_dir;

/* Structure for passing BuildChar and BuildGlyph procedures. */
typedef struct build_proc_refs_s {
    ref BuildChar;
    ref BuildGlyph;
} build_proc_refs;

/* Options for collecting parameters from a font dictionary. */
/* The comment indicates where the option is tested. */
typedef enum {
    bf_options_none = 0,
    bf_Encoding_optional = 1,	/* build_gs_font */
    bf_UniqueID_ignored = 4,	/* build_gs_simple_font */
    bf_CharStrings_optional = 8,	/* build_gs_primitive_font */
    bf_notdef_required = 16	/* build_gs_primitive_font */
} build_font_options_t;

/* In zbfont.c */
int build_proc_name_refs(P3(build_proc_refs * pbuild,
			    const char *bcstr,
			    const char *bgstr));
int build_gs_font_procs(P2(os_ptr, build_proc_refs *));
#define BUILD_BASE_FONT_PROC(proc)\
  int proc(P7(i_ctx_t *, os_ptr, gs_font_base **, font_type,\
	      gs_memory_type_ptr_t, const build_proc_refs *,\
	      build_font_options_t))
typedef BUILD_BASE_FONT_PROC((*build_base_font_proc_t));
BUILD_BASE_FONT_PROC(build_gs_primitive_font);
int build_gs_FDArray_font(P6(i_ctx_t *, /*const*/ ref *, gs_font_base **,
			     font_type, gs_memory_type_ptr_t,
			     const build_proc_refs *));
int build_gs_outline_font(P8(i_ctx_t *, os_ptr, gs_font_base **, font_type,
			     gs_memory_type_ptr_t, const build_proc_refs *,
			     build_font_options_t, build_base_font_proc_t));
BUILD_BASE_FONT_PROC(build_gs_simple_font);
void init_gs_simple_font(P3(gs_font_base *pfont, const double bbox[4],
			    const gs_uid *puid));
void lookup_gs_simple_font_encoding(P1(gs_font_base *));
int build_gs_font(P7(i_ctx_t *, os_ptr, gs_font **, font_type,
		     gs_memory_type_ptr_t, const build_proc_refs *,
		     build_font_options_t));
int build_gs_sub_font(P8(i_ctx_t *, const ref *, gs_font **,
			 font_type, gs_memory_type_ptr_t,
			 const build_proc_refs *, const ref *, ref *));
int define_gs_font(P1(gs_font *));
gs_glyph zfont_encode_char(P3(gs_font *pfont, gs_char chr, gs_glyph_space_t ignored));

#endif /* bfont_INCLUDED */
