/* Copyright (C) 1998, 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: idstack.h,v 1.2 2004/02/14 22:20:19 atai Exp $ */
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
