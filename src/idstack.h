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

/* $Id: idstack.h,v 1.7 2007/09/11 15:24:34 Arabidopsis Exp $ */
/* Generic dictionary stack API */

#ifndef idstack_INCLUDED
#  define idstack_INCLUDED

#include "iddstack.h"
#include "idsdata.h"
#include "istack.h"

/* Define the type of pointers into the dictionary stack. */
typedef s_ptr ds_ptr;
typedef const_s_ptr const_ds_ptr;

/* Clean up a dictionary stack after a garbage collection. */
void dstack_gc_cleanup(dict_stack_t *);

/*
 * Define a special fast entry for name lookup on a dictionary stack.
 * The key is known to be a name; search the entire dict stack.
 * Return the pointer to the value slot.
 * If the name isn't found, just return 0.
 */
ref *dstack_find_name_by_index(dict_stack_t *, uint);

/*
 * Define an extra-fast macro for name lookup, optimized for
 * a single-probe lookup in the top dictionary on the stack.
 * Amazingly enough, this seems to hit over 90% of the time
 * (aside from operators, of course, which are handled either with
 * the special cache pointer or with 'bind').
 */
#define dstack_find_name_by_index_inline(pds,nidx,htemp)\
  ((pds)->top_keys[htemp = dict_hash_mod_inline(dict_name_index_hash(nidx),\
     (pds)->top_npairs) + 1] == pt_tag(pt_literal_name) + (nidx) ?\
   (pds)->top_values + htemp : dstack_find_name_by_index(pds, nidx))
/*
 * Define a similar macro that only checks the top dictionary on the stack.
 */
#define if_dstack_find_name_by_index_top(pds,nidx,htemp,pvslot)\
  if ( (((pds)->top_keys[htemp = dict_hash_mod_inline(dict_name_index_hash(nidx),\
	 (pds)->top_npairs) + 1] == pt_tag(pt_literal_name) + (nidx)) ?\
	((pvslot) = (pds)->top_values + (htemp), 1) :\
	0)\
     )

#endif /* idstack_INCLUDED */
