/* Copyright (C) 1997 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gxcomp.h,v 1.2 2004/02/14 22:20:18 atai Exp $ */
/* Definitions for implementing compositing functions */

#ifndef gxcomp_INCLUDED
#  define gxcomp_INCLUDED

#include "gscompt.h"
#include "gsrefct.h"
#include "gxbitfmt.h"

/*
 * Because compositor information is passed through the command list,
 * individual compositors must be identified by some means that is
 * independent of address space. The address of the compositor method
 * array, gs_composite_type_t (see below), cannot be used, as it is
 * meaningful only within a single address space.
 *
 * In addition, it is desirable the keep the compositor identifier
 * size as small as possible, as this identifier must be passed through
 * the command list for all bands whenever the compositor is invoked.
 * Fortunately, compositor invocation is not so frequent as to warrant
 * byte-sharing techniques, which most likely would store the identifier
 * in some unused bits of a command code byte. Hence, the smallest
 * reasonable size for the identifier is one byte. This allows for up
 * to 255 compositors, which should be ample (as of this writing, there
 * are only two compositors, only one of which can be passed through
 * the command list).
 *
 * The following list is intended to enumerate all compositors. We
 * use definitions rather than an encoding to ensure a one-byte size.
 */
#define GX_COMPOSITOR_ALPHA      0x01   /* DPS/Next alpha compositor */
#define GX_COMPOSITOR_OVERPRINT  0x02   /* overprint/overprintmode compositor */


/*
 * Define the abstract superclass for all compositing function types.
 */
						   /*typedef struct gs_composite_s gs_composite_t; *//* in gscompt.h */

#ifndef gs_imager_state_DEFINED
#  define gs_imager_state_DEFINED
typedef struct gs_imager_state_s gs_imager_state;

#endif

#ifndef gx_device_DEFINED
#  define gx_device_DEFINED
typedef struct gx_device_s gx_device;

#endif

typedef struct gs_composite_type_procs_s {

    /*
     * Create the default compositor for a compositing function.
     */
#define composite_create_default_compositor_proc(proc)\
  int proc(const gs_composite_t *pcte, gx_device **pcdev,\
    gx_device *dev, const gs_imager_state *pis, gs_memory_t *mem)
    composite_create_default_compositor_proc((*create_default_compositor));

    /*
     * Test whether this function is equal to another one.
     */
#define composite_equal_proc(proc)\
  bool proc(const gs_composite_t *pcte, const gs_composite_t *pcte2)
    composite_equal_proc((*equal));

    /*
     * Convert the representation of this function to a string
     * for writing in a command list.  *psize is the amount of space
     * available.  If it is large enough, the procedure sets *psize
     * to the amount used and returns 0; if it is not large enough,
     * the procedure sets *psize to the amount needed and returns a
     * rangecheck error; in the case of any other error, *psize is
     * not changed.
     */
#define composite_write_proc(proc)\
  int proc(const gs_composite_t *pcte, byte *data, uint *psize)
    composite_write_proc((*write));

    /*
     * Convert the string representation of a function back to
     * a structure, allocating the structure. Return the number of
     * bytes read, or < 0 in the event of an error.
     */
#define composite_read_proc(proc)\
  int proc(gs_composite_t **ppcte, const byte *data, uint size,\
    gs_memory_t *mem)
    composite_read_proc((*read));

} gs_composite_type_procs_t;
typedef struct gs_composite_type_s {
    byte comp_id;   /* to identify compositor passed through command list */
    gs_composite_type_procs_t procs;
} gs_composite_type_t;

/*
 * Compositing objects are reference-counted, because graphics states will
 * eventually reference them.  Note that the common part has no
 * garbage-collectible pointers and is never actually instantiated, so no
 * structure type is needed for it.
 */
#define gs_composite_common\
	const gs_composite_type_t *type;\
	gs_id id;		/* see gscompt.h */\
	rc_header rc
struct gs_composite_s {
    gs_composite_common;
};

/* Replace a procedure with a macro. */
#define gs_composite_id(pcte) ((pcte)->id)

#endif /* gxcomp_INCLUDED */