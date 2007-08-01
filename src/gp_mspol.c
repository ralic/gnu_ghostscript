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

/* $Id: gp_mspol.c,v 1.7 2007/08/01 14:25:59 jemarch Exp $ */
/*
 * Microsoft Windows platform polling support for Ghostscript.
 *
 */

#include "gx.h"
#include "gp.h"
#include "gpcheck.h"
#include "iapi.h"
#include "iref.h"
#include "iminst.h"
#include "imain.h"

/* ------ Process message loop ------ */
/* 
 * Check messages and interrupts; return true if interrupted.
 * This is called frequently - it must be quick!
 */
#ifdef CHECK_INTERRUPTS
int
gp_check_interrupts(const gs_memory_t *mem)
{
    if(mem == NULL) {
	/* MAJOR HACK will NOT work in multithreaded environment */
	mem = gs_lib_ctx_get_non_gc_memory_t();
    }
    if (mem && mem->gs_lib_ctx && mem->gs_lib_ctx->poll_fn)
	return (*mem->gs_lib_ctx->poll_fn)(mem->gs_lib_ctx->caller_handle);
    return 0;
}
#endif
