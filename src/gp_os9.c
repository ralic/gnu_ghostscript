/* Copyright (C) 1989, 2000 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gp_os9.c,v 1.2 2004/02/14 22:20:16 atai Exp $ */
/* OSK-specific routines for Ghostscript */
#include "pipe_.h"
#include "string_.h"
#include "time_.h"
#include "gx.h"
#include "gp.h"
#include <signal.h>
#include <stdlib.h>		/* for exit */
#include <sys/param.h>		/* for MAXPATHLEN */

int interrupted;

/* Forward declarations */
private void signalhandler(int);
private FILE *rbfopen(char *, char *);

/* Do platform-dependent initialization */
void
gp_init(void)
{
    intercept(signalhandler);
}

/* Do platform-dependent cleanup. */
void
gp_exit(int exit_status, int code)
{
}

/* Exit the program. */
void
gp_do_exit(int exit_status)
{
    exit(exit_status);
}

private void
signalhandler(int sig)
{
    clearerr(stdin);
    switch (sig) {
	case SIGINT:
	case SIGQUIT:
	    interrupted = 1;
	    break;
	case SIGFPE:
	    interrupted = 2;
	    break;
	default:
	    break;
    }
}

/* ------ Date and time ------ */

/* Read the current time (in seconds since Jan. 1, 1980) */
/* and fraction (in nanoseconds). */
#define PS_YEAR_0 80
#define PS_MONTH_0 1
#define PS_DAY_0 1
void
gp_get_realtime(long *pdt)
{
    long date, time, pstime, psdate, tick;
    short day;

    _sysdate(0, &time, &date, &day, &tick);
    _julian(&time, &date);

    pstime = 0;
    psdate = (PS_YEAR_0 << 16) + (PS_MONTH_0 << 8) + PS_DAY_0;
    _julian(&pstime, &psdate);

    pdt[0] = (date - psdate) * 86400 + time;
    pdt[1] = 0;

#ifdef DEBUG_CLOCK
    printf("pdt[0] = %ld  pdt[1] = %ld\n", pdt[0], pdt[1]);
#endif
}

/* Read the current user CPU time (in seconds) */
/* and fraction (in nanoseconds).  */
void
gp_get_usertime(long *pdt)
{
    return gp_get_realtime(pdt);	/* not yet implemented */
}

/* ------ Printer accessing ------ */

/* Open a connection to a printer.  A null file name means use the */
/* standard printer connected to the machine, if any. */
/* "|command" opens an output pipe. */
/* Return NULL if the connection could not be opened. */
FILE *
gp_open_printer(char fname[gp_file_name_sizeof], int binary_mode)
{
    return
	(strlen(fname) == 0 ? 0 :
	 fname[0] == '|' ? popen(fname + 1, "w") :
	 rbfopen(fname, "w"));
}

FILE *
rbfopen(char *fname, char *perm)
{
    FILE *file = fopen(fname, perm);

    file->_flag |= _RBF;
    return file;
}

/* Close the connection to the printer. */
void
gp_close_printer(FILE * pfile, const char *fname)
{
    if (fname[0] == '|')
	pclose(pfile);
    else
	fclose(pfile);
}

/* ------ File accessing -------- */

/* Set a file into binary or text mode. */
int
gp_setmode_binary(FILE * pfile, bool binary)
{
    if (binary)
	file->_flag |= _RBF;
    else
	file->_flag &= ~_RBF;
    return 0;
}