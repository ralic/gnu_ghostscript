/* Copyright (C) 1993, 1996, 1997, 1998, 1999, 2001 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gsmemory.h,v 1.2 2004/02/14 22:20:17 atai Exp $ */
/* Client interface for memory allocation */

/*
 * The allocator knows about two basic kinds of memory: objects, which are
 * aligned and cannot have pointers to their interior, and strings, which
 * are not aligned and which can have interior references.
 *
 * Note: OBJECTS ARE NOT GUARANTEED to be aligned any more strictly than
 * required by the hardware, regardless of the value of obj_align_mod.  In
 * other words, whether ALIGNMENT_MOD(ptr, obj_align_mod) will be zero
 * depends on the alignment provided by the underlying allocator.
 * Most systems ensure this, but Microsoft VC 6 in particular does not.
 * See gsmemraw.h for more information about this.
 *
 * The standard allocator is designed to interface to a garbage collector,
 * although it does not include or call one.  The allocator API recognizes
 * that the garbage collector may move objects, relocating pointers to them;
 * the API provides for allocating both movable (the default) and immovable
 * objects.  Clients must not attempt to resize immovable objects, and must
 * not create references to substrings of immovable strings.
 */

#ifndef gsmemory_INCLUDED
#  define gsmemory_INCLUDED

#include "gsmemraw.h"
#include "gstypes.h"		/* for gs_bytestring */

/* Define the opaque type for a structure descriptor. */
typedef struct gs_memory_struct_type_s gs_memory_struct_type_t;
typedef const gs_memory_struct_type_t *gs_memory_type_ptr_t;

/* Define the opaque type for an allocator. */
/* (The actual structure is defined later in this file.) */
#ifndef gs_memory_DEFINED
#  define gs_memory_DEFINED
typedef struct gs_memory_s gs_memory_t;
#endif

/* Define the opaque type for a pointer type. */
typedef struct gs_ptr_procs_s gs_ptr_procs_t;
typedef const gs_ptr_procs_t *gs_ptr_type_t;

/* Define the opaque type for a GC root. */
typedef struct gs_gc_root_s gs_gc_root_t;

	/* Accessors for structure types. */

typedef client_name_t struct_name_t;

/* Get the size of a structure from the descriptor. */
uint gs_struct_type_size(gs_memory_type_ptr_t);

/* Get the name of a structure from the descriptor. */
struct_name_t gs_struct_type_name(gs_memory_type_ptr_t);

#define gs_struct_type_name_string(styp)\
  ((const char *)gs_struct_type_name(styp))

/*
 * Define the memory manager procedural interface.
 */
typedef struct gs_memory_procs_s {

    gs_raw_memory_procs(gs_memory_t);	/* defined in gsmemraw.h */

    /* Redefine inherited procedures with the new allocator type. */

#define gs_memory_proc_alloc_bytes(proc)\
  gs_memory_t_proc_alloc_bytes(proc, gs_memory_t)
#define gs_memory_proc_resize_object(proc)\
  gs_memory_t_proc_resize_object(proc, gs_memory_t)
#define gs_memory_proc_free_object(proc)\
  gs_memory_t_proc_free_object(proc, gs_memory_t)
#define gs_memory_proc_stable(proc)\
  gs_memory_t_proc_stable(proc, gs_memory_t)
#define gs_memory_proc_status(proc)\
  gs_memory_t_proc_status(proc, gs_memory_t)
#define gs_memory_proc_free_all(proc)\
  gs_memory_t_proc_free_all(proc, gs_memory_t)
#define gs_memory_proc_consolidate_free(proc)\
  gs_memory_t_proc_consolidate_free(proc, gs_memory_t)

    /*
     * Allocate possibly movable bytes.  (We inherit allocating immovable
     * bytes from the raw memory allocator.)
     */

#define gs_alloc_bytes(mem, nbytes, cname)\
  (*(mem)->procs.alloc_bytes)(mem, nbytes, cname)
    gs_memory_proc_alloc_bytes((*alloc_bytes));

    /*
     * Allocate a structure.
     */

#define gs_memory_proc_alloc_struct(proc)\
  void *proc(gs_memory_t *mem, gs_memory_type_ptr_t pstype,\
    client_name_t cname)
#define gs_alloc_struct(mem, typ, pstype, cname)\
  (typ *)(*(mem)->procs.alloc_struct)(mem, pstype, cname)
    gs_memory_proc_alloc_struct((*alloc_struct));
#define gs_alloc_struct_immovable(mem, typ, pstype, cname)\
  (typ *)(*(mem)->procs.alloc_struct_immovable)(mem, pstype, cname)
    gs_memory_proc_alloc_struct((*alloc_struct_immovable));

    /*
     * Allocate an array of bytes.
     */

#define gs_memory_proc_alloc_byte_array(proc)\
  byte *proc(gs_memory_t *mem, uint num_elements, uint elt_size,\
    client_name_t cname)
#define gs_alloc_byte_array(mem, nelts, esize, cname)\
  (*(mem)->procs.alloc_byte_array)(mem, nelts, esize, cname)
    gs_memory_proc_alloc_byte_array((*alloc_byte_array));
#define gs_alloc_byte_array_immovable(mem, nelts, esize, cname)\
  (*(mem)->procs.alloc_byte_array_immovable)(mem, nelts, esize, cname)
    gs_memory_proc_alloc_byte_array((*alloc_byte_array_immovable));

    /*
     * Allocate an array of structures.
     */

#define gs_memory_proc_alloc_struct_array(proc)\
  void *proc(gs_memory_t *mem, uint num_elements,\
    gs_memory_type_ptr_t pstype, client_name_t cname)
#define gs_alloc_struct_array(mem, nelts, typ, pstype, cname)\
  (typ *)(*(mem)->procs.alloc_struct_array)(mem, nelts, pstype, cname)
    gs_memory_proc_alloc_struct_array((*alloc_struct_array));
#define gs_alloc_struct_array_immovable(mem, nelts, typ, pstype, cname)\
 (typ *)(*(mem)->procs.alloc_struct_array_immovable)(mem, nelts, pstype, cname)
    gs_memory_proc_alloc_struct_array((*alloc_struct_array_immovable));

    /*
     * Get the size of an object (anything except a string).
     */

#define gs_memory_proc_object_size(proc)\
  uint proc(gs_memory_t *mem, const void *obj)
#define gs_object_size(mem, obj)\
  (*(mem)->procs.object_size)(mem, obj)
    gs_memory_proc_object_size((*object_size));

    /*
     * Get the type of an object (anything except a string).
     * The value returned for byte objects is useful only for
     * printing.
     */

#define gs_memory_proc_object_type(proc)\
  gs_memory_type_ptr_t proc(gs_memory_t *mem, const void *obj)
#define gs_object_type(mem, obj)\
  (*(mem)->procs.object_type)(mem, obj)
    gs_memory_proc_object_type((*object_type));

    /*
     * Allocate a string (unaligned bytes).
     */

#define gs_memory_proc_alloc_string(proc)\
  byte *proc(gs_memory_t *mem, uint nbytes, client_name_t cname)
#define gs_alloc_string(mem, nbytes, cname)\
  (*(mem)->procs.alloc_string)(mem, nbytes, cname)
    gs_memory_proc_alloc_string((*alloc_string));
#define gs_alloc_string_immovable(mem, nbytes, cname)\
  (*(mem)->procs.alloc_string_immovable)(mem, nbytes, cname)
    gs_memory_proc_alloc_string((*alloc_string_immovable));

    /*
     * Resize a string.  The specification is the same as resize_object
     * (in gsmemraw.h), except that the element size is always a byte.
     */

#define gs_memory_proc_resize_string(proc)\
  byte *proc(gs_memory_t *mem, byte *data, uint old_num, uint new_num,\
    client_name_t cname)
#define gs_resize_string(mem, data, oldn, newn, cname)\
  (*(mem)->procs.resize_string)(mem, data, oldn, newn, cname)
    gs_memory_proc_resize_string((*resize_string));

    /*
     * Free a string.
     */

#define gs_memory_proc_free_string(proc)\
  void proc(gs_memory_t *mem, byte *data, uint nbytes,\
    client_name_t cname)
#define gs_free_string(mem, data, nbytes, cname)\
  (*(mem)->procs.free_string)(mem, data, nbytes, cname)
    gs_memory_proc_free_string((*free_string));

    /*
     * Register a root for the garbage collector.  root = NULL
     * asks the memory manager to allocate the root object
     * itself (immovable, in the manager's parent): this is the usual
     * way to call this procedure.
     */

#define gs_memory_proc_register_root(proc)\
  int proc(gs_memory_t *mem, gs_gc_root_t *root, gs_ptr_type_t ptype,\
    void **pp, client_name_t cname)
#define gs_register_root(mem, root, ptype, pp, cname)\
  (*(mem)->procs.register_root)(mem, root, ptype, pp, cname)
    gs_memory_proc_register_root((*register_root));

    /*
     * Unregister a root.  The root object itself will be freed iff
     * it was allocated by gs_register_root.
     */

#define gs_memory_proc_unregister_root(proc)\
  void proc(gs_memory_t *mem, gs_gc_root_t *root, client_name_t cname)
#define gs_unregister_root(mem, root, cname)\
  (*(mem)->procs.unregister_root)(mem, root, cname)
    gs_memory_proc_unregister_root((*unregister_root));

    /*
     * Enable or disable the freeing operations: when disabled,
     * these operations return normally but do nothing.  The
     * garbage collector and the PostScript interpreter
     * 'restore' operator need to temporarily disable the
     * freeing functions of (an) allocator(s) while running
     * finalization procedures.
     */

#define gs_memory_proc_enable_free(proc)\
  void proc(gs_memory_t *mem, bool enable)
#define gs_enable_free(mem, enable)\
  (*(mem)->procs.enable_free)(mem, enable)
    gs_memory_proc_enable_free((*enable_free));

} gs_memory_procs_t;

/*
 * Define versions of the freeing procedures that are applicable even if the
 * pointer is declared as const T *.  These are intended for use where a
 * structure contains a pointer member whose referent is declared as const
 * because it is const for all ordinary clients.
 */
void gs_free_const_object(gs_memory_t *mem, const void *data,
			  client_name_t cname);
void gs_free_const_string(gs_memory_t *mem, const byte *data, uint nbytes,
			  client_name_t cname);

/*
 * Free a [const] bytestring.  Note that this is *not* a member procedure of
 * the allocator: it calls the free_object or free_string procedure.
 */
void gs_free_bytestring(gs_memory_t *mem, gs_bytestring *pbs,
			client_name_t cname);
void gs_free_const_bytestring(gs_memory_t *mem, gs_const_bytestring *pbs,
			      client_name_t cname);

/*
 * Either allocate (if obj == 0) or resize (if obj != 0) a structure array.
 * If obj != 0, pstype is used only for checking (in DEBUG configurations).
 */
void *gs_resize_struct_array(gs_memory_t *mem, void *obj, uint num_elements,
			     gs_memory_type_ptr_t pstype,
			     client_name_t cname);

/* Register a structure root.  This just calls gs_register_root. */
int gs_register_struct_root(gs_memory_t *mem, gs_gc_root_t *root,
			    void **pp, client_name_t cname);

/* Define no-op freeing procedures for use by enable_free. */
gs_memory_proc_free_object(gs_ignore_free_object);
gs_memory_proc_free_string(gs_ignore_free_string);

/* Define a no-op consolidation procedure. */
gs_memory_proc_consolidate_free(gs_ignore_consolidate_free);

/*
 * Allocate a structure using a "raw memory" allocator.  Note that this does
 * not retain the identity of the structure.  Note also that it returns a
 * void *, and does not take the type of the returned pointer as a
 * parameter.
 */
void *gs_raw_alloc_struct_immovable(gs_raw_memory_t * rmem,
				    gs_memory_type_ptr_t pstype,
				    client_name_t cname);

/*
 * Define an abstract allocator instance.
 * Subclasses may have state as well.
 * Note that the initial part of this must match gs_raw_memory_t.
 */
#define gs_memory_common\
	gs_memory_t *stable_memory;\
	gs_memory_procs_t procs
struct gs_memory_s {
    gs_memory_common;
};

#endif /* gsmemory_INCLUDED */