/* Copyright (C) 1997 artofcode LLC.  All rights reserved.
  
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

/*$Id: gxclzlib.c,v 1.1 2004/01/14 16:59:51 atai Exp $ */
/* zlib filter initialization for RAM-based band lists */
/* Must be compiled with -I$(ZSRCDIR) */
#include "std.h"
#include "gstypes.h"
#include "gsmemory.h"
#include "gxclmem.h"
#include "szlibx.h"

private stream_zlib_state cl_zlibE_state;
private stream_zlib_state cl_zlibD_state;

/* Initialize the states to be copied. */
void
gs_cl_zlib_init(gs_memory_t * mem)
{
    s_zlib_set_defaults((stream_state *) & cl_zlibE_state);
    cl_zlibE_state.no_wrapper = true;
    cl_zlibE_state.template = &s_zlibE_template;
    s_zlib_set_defaults((stream_state *) & cl_zlibD_state);
    cl_zlibD_state.no_wrapper = true;
    cl_zlibD_state.template = &s_zlibD_template;
}

/* Return the prototypes for compressing/decompressing the band list. */
const stream_state *
clist_compressor_state(void *client_data)
{
    return (const stream_state *)&cl_zlibE_state;
}
const stream_state *
clist_decompressor_state(void *client_data)
{
    return (const stream_state *)&cl_zlibD_state;
}
