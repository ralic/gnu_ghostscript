/* Copyright (C) 1991, 1995, 1998, 1999, 2000 artofcode LLC.  All rights reserved.
  
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

/*$Id: gp_iwatc.c,v 1.1 2004/01/14 16:59:48 atai Exp $ */
/* Intel processor, Watcom C-specific routines for Ghostscript */
#include "dos_.h"
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include "stat_.h"
#include "string_.h"
#include "gx.h"
#include "gp.h"
#include "gpmisc.h"

/* Library routines not declared in a standard header */
extern char *mktemp(P1(char *));	/* in gp_mktmp.c */

/* Define a substitute for stdprn (see below). */
private FILE *gs_stdprn;

/* Forward declarations */
private void handle_FPE(P1(int));

/* Do platform-dependent initialization. */
void
gp_init(void)
{
    gs_stdprn = 0;
    /* Set up the handler for numeric exceptions. */
    signal(SIGFPE, handle_FPE);
}

/* Trap numeric exceptions.  Someday we will do something */
/* more appropriate with these. */
private void
handle_FPE(int sig)
{
    eprintf("Numeric exception:\n");
    exit(1);
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
}

/* ------ Printer accessing ------ */

/* Open a connection to a printer.  A null file name means use the */
/* standard printer connected to the machine, if any. */
/* Return NULL if the connection could not be opened. */
extern void gp_set_file_binary(P2(int, int));
FILE *
gp_open_printer(char fname[gp_file_name_sizeof], int binary_mode)
{
    FILE *pfile;

    if (strlen(fname) == 0 || !strcmp(fname, "PRN")) {
#ifdef stdprn
	if (!binary_mode)
	    return stdprn;
	if (gs_stdprn == 0) {
	    /* We have to effectively reopen the printer, */
	    /* because the Watcom library does \n -> \r\n */
	    /* substitution on the stdprn stream. */
	    int fno = dup(fileno(stdprn));

	    setmode(fno, O_BINARY);
	    gs_stdprn = fdopen(fno, "wb");
	}
	pfile = gs_stdprn;
#else	/* WATCOM doesn't know about stdprn device */
	pfile = fopen("PRN", (binary_mode ? "wb" : "w"));
	if (pfile == NULL)
	    return NULL;
#endif	/* defined(stdprn) */
    } else {
	pfile = fopen(fname, (binary_mode ? "wb" : "w"));
	if (pfile == NULL)
	    return NULL;
    }
    gp_set_file_binary(fileno(pfile), binary_mode);
    return pfile;
}

/* Close the connection to the printer. */
void
gp_close_printer(FILE * pfile, const char *fname)
{
#ifdef stdprn
    if (pfile != stdprn)
#endif	/* defined(stdprn) */
	fclose(pfile);
    if (pfile == gs_stdprn)
	gs_stdprn = 0;
}

/* ------ File naming and accessing ------ */

/* Create and open a scratch file with a given name prefix. */
/* Write the actual file name at fname. */
FILE *
gp_open_scratch_file(const char *prefix, char *fname, const char *mode)
{	      /* The -7 is for XXXXXXX */
    int prefix_length = strlen(prefix);
    int len = gp_file_name_sizeof - prefix_length - 7;

    if (gp_pathstring_not_bare(prefix, prefix_length) ||
	gp_gettmpdir(fname, &len) != 0
	)
	*fname = 0;
    else {
	char *temp;

	/* Prevent X's in path from being converted by mktemp. */
	for (temp = fname; *temp; temp++)
	    *temp = tolower(*temp);
	if (strlen(fname) && (fname[strlen(fname) - 1] != '\\'))
	    strcat(fname, "\\");
    }
    if (strlen(fname) + prefix_length + 7 >= gp_file_name_sizeof)
	return 0;		/* file name too long */
    strcat(fname, prefix);
    strcat(fname, "XXXXXX");
    mktemp(fname);
    return gp_fopentemp(fname, mode);
}


/* Open a file with the given name, as a stream of uninterpreted bytes. */
FILE *
gp_fopen(const char *fname, const char *mode)
{
    return fopen(fname, mode);
}
