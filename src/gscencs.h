/* Copyright (C) 2001-2006 artofcode LLC.
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

/* $Id: gscencs.h,v 1.6 2007/08/01 14:26:02 jemarch Exp $ */
/* Compact C representation of built-in encodings */

#ifndef gscencs_INCLUDED
#  define gscencs_INCLUDED

#include "stdpre.h"
#include "gstypes.h"
#include "gsccode.h"

/*
 * This file defines an interface to a compact C representation of the
 * built-in encodings.  This representation is currently used only by
 * pdfwrite, but the PostScript interpreter could in principle use it too.
 *
 * This representation uses its own private numbering system for glyphs.
 * gs_c_known_encode returns one of these private glyph numbers;
 * gs_c_glyph_name converts a private glyph number to a string.
 * For more information about glyph numbering, see gsccode.h.
 *
 * The data file for this representation, gscedata.c, is generated by a
 * PostScript program, encs2c.ps.  If the representation changes, this file
 * (src/gscencs.h), gscencs.c, and lib/encs2c.ps must be kept consistent.
 */

/*
 * Define the minimum gs_glyph value for glyphs in these encodings.
 * gs_glyph values from this value through gs_min_cid_glyph - 1 are reserved.
 */
extern const gs_glyph gs_c_min_std_encoding_glyph;

/*
 * Encode a character in a known encoding.  The only use for glyph numbers
 * returned by this procedure is to pass them to gs_c_glyph_name.
 */
gs_glyph gs_c_known_encode(gs_char chr, int encoding_index);

/*
 * Decode a gs_c_glyph_name glyph with a known encoding.
 */
gs_char gs_c_decode(gs_glyph glyph, int ei);

/*
 * Convert a glyph number returned by gs_c_known_encode to a string.
 */
int gs_c_glyph_name(gs_glyph glyph, gs_const_string *pstr);

/*
 * Test whether a string is one that was returned by gs_c_glyph_name.
 */
bool gs_is_c_glyph_name(const byte *str, uint len);

/*
 * Return the glyph number corresponding to a string (the inverse of
 * gs_c_glyph_name), or GS_NO_GLYPH if the glyph name is not known.
 */
gs_glyph gs_c_name_glyph(const byte *str, uint len);

#endif /* gscencs_INCLUDED */
