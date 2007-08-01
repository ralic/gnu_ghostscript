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

/* $Id: gxclfile.c,v 1.6 2007/08/01 14:26:17 jemarch Exp $ */
/* File-based command list implementation */
#include "stdio_.h"
#include "string_.h"
#include "unistd_.h"
#include "gserror.h"
#include "gserrors.h"
#include "gsmemory.h"
#include "gp.h"
#include "gxclio.h"

/* This is an implementation of the command list I/O interface */
/* that uses the file system for storage. */

/* ------ Open/close/unlink ------ */

int
clist_fopen(char fname[gp_file_name_sizeof], const char *fmode,
	    clist_file_ptr * pcf, gs_memory_t * mem, gs_memory_t *data_mem,
	    bool ok_to_compress)
{
    if (*fname == 0) {
	if (fmode[0] == 'r')
	    return_error(gs_error_invalidfileaccess);
	*pcf =
	    (clist_file_ptr) gp_open_scratch_file_64(gp_scratch_file_name_prefix,
						  fname, fmode);
    } else
	*pcf = gp_fopen(fname, fmode);
    if (*pcf == NULL) {
	eprintf1("Could not open the scratch file %s.\n", fname);
	return_error(gs_error_invalidfileaccess);
    }
    return 0;
}

int
clist_fclose(clist_file_ptr cf, const char *fname, bool delete)
{
    return (fclose((FILE *) cf) != 0 ? gs_note_error(gs_error_ioerror) :
	    delete ? clist_unlink(fname) :
	    0);
}

int
clist_unlink(const char *fname)
{
    return (unlink(fname) != 0 ? gs_note_error(gs_error_ioerror) : 0);
}

/* ------ Writing ------ */

long
clist_space_available(long requested)
{
    return requested;
}

int
clist_fwrite_chars(const void *data, uint len, clist_file_ptr cf)
{
    return fwrite(data, 1, len, (FILE *) cf);
}

/* ------ Reading ------ */

int
clist_fread_chars(void *data, uint len, clist_file_ptr cf)
{
    FILE *f = (FILE *) cf;
    byte *str = data;

    /* The typical implementation of fread */
    /* is extremely inefficient for small counts, */
    /* so we just use straight-line code instead. */
    switch (len) {
	default:
	    return fread(str, 1, len, f);
	case 8:
	    *str++ = (byte) getc(f);
	case 7:
	    *str++ = (byte) getc(f);
	case 6:
	    *str++ = (byte) getc(f);
	case 5:
	    *str++ = (byte) getc(f);
	case 4:
	    *str++ = (byte) getc(f);
	case 3:
	    *str++ = (byte) getc(f);
	case 2:
	    *str++ = (byte) getc(f);
	case 1:
	    *str = (byte) getc(f);
    }
    return len;
}

/* ------ Position/status ------ */

int
clist_set_memory_warning(clist_file_ptr cf, int bytes_left)
{
    return 0;			/* no-op */
}

int
clist_ferror_code(clist_file_ptr cf)
{
    return (ferror((FILE *) cf) ? gs_error_ioerror : 0);
}

int64_t
clist_ftell(clist_file_ptr cf)
{
    return gp_ftell_64((FILE *) cf);
}

void
clist_rewind(clist_file_ptr cf, bool discard_data, const char *fname)
{
    FILE *f = (FILE *) cf;

    if (discard_data) {
	/*
	 * The ANSI C stdio specification provides no operation for
	 * truncating a file at a given position, or even just for
	 * deleting its contents; we have to use a bizarre workaround to
	 * get the same effect.
	 */
	char fmode[4];

	/* Opening with "w" mode deletes the contents when closing. */
	freopen(fname, gp_fmode_wb, f);
	strcpy(fmode, "w+");
	strcat(fmode, gp_fmode_binary_suffix);
	freopen(fname, fmode, f);
    } else {
	rewind(f);
    }
}

int
clist_fseek(clist_file_ptr cf, int64_t offset, int mode, const char *ignore_fname)
{
    return gp_fseek_64((FILE *) cf, offset, mode);
}
