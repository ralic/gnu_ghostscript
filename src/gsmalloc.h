/* Copyright (C) 1997, 1998, 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gsmalloc.h,v 1.2 2004/02/14 22:20:17 atai Exp $ */
/* Client interface to default (C heap) allocator */
/* Requires gsmemory.h */

#ifndef gsmalloc_INCLUDED
#  define gsmalloc_INCLUDED

/* Define a memory manager that allocates directly from the C heap. */
typedef struct gs_malloc_block_s gs_malloc_block_t;
typedef struct gs_malloc_memory_s {
    gs_memory_common;
    gs_malloc_block_t *allocated;
    long limit;
    long used;
    long max_used;
} gs_malloc_memory_t;

/* Allocate and initialize a malloc memory manager. */
gs_malloc_memory_t *gs_malloc_memory_init(void);

/* Release all the allocated blocks, and free the memory manager. */
/* The cast is unfortunate, but unavoidable. */
#define gs_malloc_memory_release(mem)\
  gs_memory_free_all((gs_memory_t *)mem, FREE_ALL_EVERYTHING,\
		     "gs_malloc_memory_release")

/*
 * Define a default allocator that allocates from the C heap.
 * (We would really like to get rid of this.)
 */
extern gs_malloc_memory_t *gs_malloc_memory_default;
extern gs_memory_t *gs_memory_t_default;  /* may be locked */
#define gs_memory_default (*gs_memory_t_default)

/*
 * The following procedures are historical artifacts that we hope to
 * get rid of someday.
 */
gs_memory_t * gs_malloc_init(void);
void gs_malloc_release(void);
#define gs_malloc(nelts, esize, cname)\
  (void *)gs_alloc_byte_array(&gs_memory_default, nelts, esize, cname)
#define gs_free(data, nelts, esize, cname)\
  gs_free_object(&gs_memory_default, data, cname)

/* Define an accessor for the limit on the total allocated heap space. */
#define gs_malloc_limit (gs_malloc_memory_default->limit)

/* Define an accessor for the maximum amount ever allocated from the heap. */
#define gs_malloc_max (gs_malloc_memory_default->max_used)

/* ---------------- Locking ---------------- */

/* Create a locked wrapper for a heap allocator. */
int gs_malloc_wrap(gs_memory_t **wrapped, gs_malloc_memory_t *contents);

/* Get the wrapped contents. */
gs_malloc_memory_t *gs_malloc_wrapped_contents(gs_memory_t *wrapped);

/* Free the wrapper, and return the wrapped contents. */
gs_malloc_memory_t *gs_malloc_unwrap(gs_memory_t *wrapped);

#endif /* gsmalloc_INCLUDED */