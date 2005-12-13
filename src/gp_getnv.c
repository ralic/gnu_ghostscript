/* Copyright (C) 1997 Aladdin Enterprises.  All rights reserved.
  
  This file is part of GNU ghostscript

  GNU ghostscript is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2, or (at your option) any later version.

  GNU ghostscript is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  ghostscript; see the file COPYING. If not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*/

/* $Id: gp_getnv.c,v 1.4 2005/12/13 16:57:20 jemarch Exp $ */
/* Standard implementation of gp_getenv */
#include "stdio_.h"
#include "string_.h"
#include "gsmemory.h"
#include "gstypes.h"
#include "gp.h"

/* Import the C getenv function. */
extern char *getenv(const char *);

/* Get the value of an environment variable.  See gp.h for details. */
int
gp_getenv(const char *key, char *ptr, int *plen)
{
    const char *str = getenv(key);

    if (str) {
	int len = strlen(str);

	if (len < *plen) {
	    /* string fits */
	    strcpy(ptr, str);
	    *plen = len + 1;
	    return 0;
	}
	/* string doesn't fit */
	*plen = len + 1;
	return -1;
    }
    /* missing key */
    if (*plen > 0)
	*ptr = 0;
    *plen = 1;
    return 1;
}
