/* Copyright (C) 1993, 2000 artofcode LLC.  All rights reserved.
  
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

/*$Id: gdevpipe.c,v 1.1 2004/01/14 16:59:48 atai Exp $ */
/* %pipe% IODevice */
#include "errno_.h"
#include "pipe_.h"
#include "stdio_.h"
#include "string_.h"
#include "gserror.h"
#include "gserrors.h"
#include "gstypes.h"
#include "gsmemory.h"		/* for gxiodev.h */
#include "gxiodev.h"

/* The pipe IODevice */
private iodev_proc_fopen(pipe_fopen);
private iodev_proc_fclose(pipe_fclose);
const gx_io_device gs_iodev_pipe = {
    "%pipe%", "Special",
    {iodev_no_init, iodev_no_open_device,
     NULL /*iodev_os_open_file */ , pipe_fopen, pipe_fclose,
     iodev_no_delete_file, iodev_no_rename_file, iodev_no_file_status,
     iodev_no_enumerate_files, NULL, NULL,
     iodev_no_get_params, iodev_no_put_params
    }
};

/* The file device procedures */

private int
pipe_fopen(gx_io_device * iodev, const char *fname, const char *access,
	   FILE ** pfile, char *rfname, uint rnamelen)
{
    errno = 0;
    /*
     * Some platforms allow opening a pipe with a '+' in the access
     * mode, even though pipes are not positionable.  Detect this here.
     */
    if (strchr(access, '+'))
	return_error(gs_error_invalidfileaccess);
    /*
     * The OSF/1 1.3 library doesn't include const in the
     * prototype for popen, so we have to break const here.
     */
    *pfile = popen((char *)fname, (char *)access);
    if (*pfile == NULL)
	return_error(gs_fopen_errno_to_code(errno));
    if (rfname != NULL)
	strcpy(rfname, fname);
    return 0;
}

private int
pipe_fclose(gx_io_device * iodev, FILE * file)
{
    pclose(file);
    return 0;
}
