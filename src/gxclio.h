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

/* $Id: gxclio.h,v 1.6 2007/08/01 14:26:18 jemarch Exp $ */
/* I/O interface for command lists */

#ifndef gxclio_INCLUDED
#  define gxclio_INCLUDED

#include "gp.h"			/* for gp_file_name_sizeof */

/*
 * There are two implementations of the I/O interface for command lists --
 * one suitable for embedded systems, which stores the "files" in RAM, and
 * one suitable for other systems, which uses an external file system --
 * with the choice made at compile/link time.  This header file defines the
 * API between the command list code proper and its I/O interface.
 */

typedef void *clist_file_ptr;	/* We can't do any better than this. */

/* ---------------- Open/close/unlink ---------------- */

/*
 * If *fname = 0, generate and store a new scratch file name; otherwise,
 * open an existing file.  Only modes "r" and "w+" are supported,
 * and only binary data (but the caller must append the "b" if needed).
 * Mode "r" with *fname = 0 is an error.
 */
int clist_fopen(char fname[gp_file_name_sizeof], const char *fmode,
		clist_file_ptr * pcf,
		gs_memory_t * mem, gs_memory_t *data_mem,
		bool ok_to_compress);

/*
 * Close a file, optionally deleting it.
 */
int clist_fclose(clist_file_ptr cf, const char *fname, bool delete);

/*
 * Delete a file.
 */
int clist_unlink(const char *fname);

/* ---------------- Writing ---------------- */

/* clist_space_available returns min(requested, available). */
long clist_space_available(long requested);

int clist_fwrite_chars(const void *data, uint len, clist_file_ptr cf);

/* ---------------- Reading ---------------- */

int clist_fread_chars(void *data, uint len, clist_file_ptr cf);

/* ---------------- Position/status ---------------- */

/*
 * Set the low-memory warning threshold.  clist_ferror_code will return 1
 * if fewer than this many bytes of memory are left for storing band data.
 */
int clist_set_memory_warning(clist_file_ptr cf, int bytes_left);

/*
 * clist_ferror_code returns a negative error code per gserrors.h, not a
 * Boolean; 0 means no error, 1 means low-memory warning.
 */
int clist_ferror_code(clist_file_ptr cf);

int64_t clist_ftell(clist_file_ptr cf);

/*
 * We pass the file name to clist_rewind and clist_fseek in case the
 * implementation has to close and reopen the file.  (clist_fseek with
 * offset = 0 and mode = SEEK_END indicates we are about to append.)
 */
void clist_rewind(clist_file_ptr cf, bool discard_data, const char *fname);

int clist_fseek(clist_file_ptr cf, int64_t offset, int mode, const char *fname);

#endif /* gxclio_INCLUDED */
