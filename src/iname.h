/* Copyright (C) 1989, 1995, 1998, 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: iname.h,v 1.2 2004/02/14 22:20:19 atai Exp $ */
/* Interpreter's name table interface */

#ifndef iname_INCLUDED
#  define iname_INCLUDED

#include "inames.h"

/*
 * This file defines those parts of the name table API that refer to the
 * interpreter's distinguished instance.  Procedures in this file begin
 * with name_.
 */

/* ---------------- Procedural interface ---------------- */

/* Define the interpreter's name table. */
extern name_table *the_gs_name_table;

/* Backward compatibility */
#define the_name_table() ((const name_table *)the_gs_name_table)

/* Get the allocator for the name table. */
#define name_memory()\
  names_memory(the_gs_name_table)

/*
 * Look up and/or enter a name in the name table.
 * See inames.h for the values of enterflag, and the possible return values.
 */
#define name_ref(ptr, size, pnref, enterflag)\
  names_ref(the_gs_name_table, ptr, size, pnref, enterflag)
#define name_string_ref(pnref, psref)\
  names_string_ref(the_gs_name_table, pnref, psref)
/*
 * name_enter_string calls name_ref with a (permanent) C string.
 */
#define name_enter_string(str, pnref)\
  names_enter_string(the_gs_name_table,str, pnref)
/*
 * name_from_string essentially implements cvn.
 * It always enters the name, and copies the executable attribute.
 */
#define name_from_string(psref, pnref)\
  names_from_string(the_gs_name_table, psref, pnref)

/* Compare two names for equality. */
#define name_eq(pnref1, pnref2)\
  names_eq(pnref1, pnref2)

/* Invalidate the value cache for a name. */
#define name_invalidate_value_cache(pnref)\
  names_invalidate_value_cache(the_gs_name_table, pnref)

/* Convert between names and indices. */
#define name_index(pnref)		/* ref => index */\
  names_index(the_gs_name_table, pnref)
#define name_index_ptr(nidx)		/* index => name */\
  names_index_ptr(the_gs_name_table, nidx)
#define name_index_ref(nidx, pnref)	/* index => ref */\
  names_index_ref(the_gs_name_table, nidx, pnref)

/* Get the index of the next valid name. */
/* The argument is 0 or a valid index. */
/* Return 0 if there are no more. */
#define name_next_valid_index(nidx)\
  names_next_valid_index(the_gs_name_table, nidx)

/* Mark a name for the garbage collector. */
/* Return true if this is a new mark. */
#define name_mark_index(nidx)\
  names_mark_index(the_gs_name_table, nidx)

/* Get the object (sub-table) containing a name. */
/* The garbage collector needs this so it can relocate pointers to names. */
#define name_ref_sub_table(pnref)\
  names_ref_sub_table(the_gs_name_table, pnref)

#endif /* iname_INCLUDED */