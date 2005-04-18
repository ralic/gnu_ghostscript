/* Copyright (C) 2001 Artifex Software, Inc.  All rights reserved.
  
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

/* $Id: sarc4.h,v 1.3 2005/04/18 12:06:03 Arabidopsis Exp $ */
/* Definitions for Arcfour cipher and filter */
/* Requires scommon.h; strimpl.h if any templates are referenced */

#ifndef sarc4_INCLUDED
#  define sarc4_INCLUDED

#include "scommon.h"

/* Arcfour is a symmetric cipher whose state is maintained
 * in two indices into an accompanying 8x8 S box. this will
 * typically be allocated on the stack, and so has no memory
 * management associated.
 */
struct stream_arcfour_state_s
{
    stream_state_common;	/* a define from scommon.h */
    unsigned int x, y;
    unsigned char S[256];
};

#ifndef stream_arcfour_state_DEFINED
#define stream_arcfour_state_DEFINED
typedef struct stream_arcfour_state_s stream_arcfour_state;
#endif

int s_arcfour_set_key(stream_arcfour_state * state, const unsigned char *key,
		      int keylength);

#define private_st_arcfour_state()	/* used in sarc4.c */\
  gs_private_st_simple(st_arcfour_state, stream_arcfour_state,\
    "Arcfour filter state")
extern const stream_template s_arcfour_template;

/* (de)crypt a section of text in a buffer -- the procedure is the same
 * in each direction. see strimpl.h for return codes.
 */
int s_arcfour_process_buffer(stream_arcfour_state *ss, byte *buf, int buf_size);

#endif /* sarc4_INCLUDED */
