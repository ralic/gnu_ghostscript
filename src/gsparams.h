/* Copyright (C) 1998 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gsparams.h,v 1.2 2004/02/14 22:20:17 atai Exp $ */
/* Serializer/expander for gs_parm_list's */

#ifndef gsparams_INCLUDED
#  define gsparams_INCLUDED

/* Initial version 2/1/98 by John Desrosiers (soho@crl.com) */
/* 8/8/98 L. Peter Deutsch (ghost@aladdin.com) Completely redesigned
   to use stream rather than buffer API (but retained former API for
   compatibility as well). */

#include "stream.h"
#include "gsparam.h"

#if 0	/****************/

/* ------ Future interface, implemented in gsparam2.c ------ */

/*
 * Serialize the contents of a gs_param_list, including sub-collections,
 * onto a stream.  The list must be in READ mode.
 */
int gs_param_list_puts(stream *dest, gs_param_list *list);

/*
 * Unserialize a parameter list, including sub-collections, from a stream.
 * The list must be in WRITE mode.
 */
int gs_param_list_gets(stream *src, gs_param_list *list, gs_memory_t *mem);

#else	/****************/

/* ------ Present interface, implemented in gsparams.c ------ */

/*
 * Serialize a parameter list into a buffer.  Return the actual number
 * of bytes required to store the list, or a negative error code.
 * The list was stored successfully iff the return value is positive and
 * less than or equal to the buffer size.  Note that the buffer may be
 * NULL, in which case nothing is stored (but the size is still returned).
 */
int gs_param_list_serialize(gs_param_list *list, byte *buf, int buf_size);

/*
 * Unserialize a parameter list from a buffer.  Return the actual number
 * of bytes occupied by the list, or a negative error code.  The buffer
 * must be void * aligned.
 */
int gs_param_list_unserialize(gs_param_list *list, const byte *buf);

#endif	/****************/

#endif /* gsparams_INCLUDED */