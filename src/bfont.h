/* Copyright (C) 2001-2006 Artifex Software, Inc.
   All Rights Reserved.
  
  This file is part of GNU ghostscript

  GNU ghostscript is free software; you can redistribute it and/or
  modify it under the terms of the version 2 of the GNU General Public
  License as published by the Free Software Foundation.

  GNU ghostscript is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  ghostscript; see the file COPYING. If not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*/

/* $Id: bfont.h,v 1.9 2007/09/11 15:24:33 Arabidopsis Exp $ */
/* Interpreter internal routines and data needed for building fonts */
/* Requires gxfont.h */

#ifndef bfont_INCLUDED
#  define bfont_INCLUDED

#include "ifont.h"

/* In zfont.c */
int add_FID(i_ctx_t *i_ctx_p, ref *pfdict, gs_font *pfont,
	    gs_ref_memory_t *imem);

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
int build_proc_name_refs(const gs_memory_t *mem, 
			 build_proc_refs * pbuild,
			 const char *bcstr, const char *bgstr);
int build_gs_font_procs(os_ptr, build_proc_refs *);
#define BUILD_BASE_FONT_PROC(proc)\
  int proc(i_ctx_t *, os_ptr, gs_font_base **, font_type,\
	   gs_memory_type_ptr_t, const build_proc_refs *,\
	   build_font_options_t)
typedef BUILD_BASE_FONT_PROC((*build_base_font_proc_t));
BUILD_BASE_FONT_PROC(build_gs_primitive_font);
int build_gs_FDArray_font(i_ctx_t *, /*const*/ ref *, gs_font_base **,
			  font_type, gs_memory_type_ptr_t,
			  const build_proc_refs *);
int build_gs_outline_font(i_ctx_t *, os_ptr, gs_font_base **, font_type,
			  gs_memory_type_ptr_t, const build_proc_refs *,
			  build_font_options_t, build_base_font_proc_t);
BUILD_BASE_FONT_PROC(build_gs_simple_font);
void init_gs_simple_font(gs_font_base *pfont, const double bbox[4],
			 const gs_uid *puid);
void lookup_gs_simple_font_encoding(gs_font_base *);
int build_gs_font(i_ctx_t *, os_ptr, gs_font **, font_type,
		  gs_memory_type_ptr_t, const build_proc_refs *,
		  build_font_options_t);
int build_gs_sub_font(i_ctx_t *, const ref *, gs_font **,
		      font_type, gs_memory_type_ptr_t,
		      const build_proc_refs *, const ref *, ref *);
int define_gs_font(gs_font *);
void get_font_name(const gs_memory_t *mem, ref *pfname, const ref *op);
void copy_font_name(gs_font_name * pfstr, const ref * pfname);
gs_glyph zfont_encode_char(gs_font *pfont, gs_char chr, gs_glyph_space_t ignored);
gs_char gs_font_map_glyph_to_unicode(gs_font *font, gs_glyph glyph);
const ref *zfont_get_to_unicode_map(gs_font_dir *dir);
void get_GlyphNames2Unicode(i_ctx_t *i_ctx_p, gs_font *pfont, ref *pdref);

#endif /* bfont_INCLUDED */
