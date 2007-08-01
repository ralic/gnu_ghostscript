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

/* $Id: gp_mktmp.c,v 1.6 2007/08/01 14:25:59 jemarch Exp $ */
/* Replacement for missing mktemp */
#include "stat_.h"
#include "string_.h"

/* This procedure simulates mktemp on platforms that don't provide it. */
char *
mktemp(char *fname)
{
    struct stat fst;
    int len = strlen(fname);
    char *end = fname + len - 6;
    
    if (len < 6 || strcmp(end, "XXXXXX"))
	return (char *)0;	/* invalid  */
    strcpy(end, "AA.AAA");

    while (stat(fname, &fst) == 0) {
	char *inc = fname + len - 1;

	while (*inc == 'Z' || *inc == '.') {
	    if (inc == end)
		return (char *)0;	/* failure */
	    if (*inc == 'Z')
		*inc = 'A';
	    --inc;
	}
	++*inc;
    }
    return fname;
}
