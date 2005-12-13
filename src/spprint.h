/* Copyright (C) 1997, 1998, 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: spprint.h,v 1.4 2005/12/13 16:57:28 jemarch Exp $ */
/* Print values in ASCII form on a stream */

#ifndef spprint_INCLUDED
#  define spprint_INCLUDED

/* Define an opaque type for streams. */
#ifndef stream_DEFINED
#  define stream_DEFINED
typedef struct stream_s stream;
#endif

/* Put a character on a stream. */
#define stream_putc(s, c) spputc(s, c)

/* Put a byte array on a stream. */
int stream_write(stream * s, const void *ptr, uint count);

/* Put a string on a stream. */
int stream_puts(stream * s, const char *str);

/*
 * Print (a) floating point number(s) using a format.  This is needed
 * because %f format always prints a fixed number of digits after the
 * decimal point, and %g format may use %e format, which PDF disallows.
 * These functions return a pointer to the next %-element of the format, or
 * to the terminating 0.
 */
const char *pprintg1(stream * s, const char *format, floatp v);
const char *pprintg2(stream * s, const char *format, floatp v1, floatp v2);
const char *pprintg3(stream * s, const char *format,
		     floatp v1, floatp v2, floatp v3);
const char *pprintg4(stream * s, const char *format,
		     floatp v1, floatp v2, floatp v3, floatp v4);
const char *pprintg6(stream * s, const char *format,
		     floatp v1, floatp v2, floatp v3, floatp v4,
		     floatp v5, floatp v6);

/*
 * The rest of these printing functions exist solely because the ANSI C
 * "standard" for functions with a variable number of arguments is not
 * implemented properly or consistently across compilers.
 */
/* Print (an) int value(s) using a format. */
const char *pprintd1(stream * s, const char *format, int v);
const char *pprintd2(stream * s, const char *format, int v1, int v2);
const char *pprintd3(stream * s, const char *format,
		     int v1, int v2, int v3);
const char *pprintd4(stream * s, const char *format,
		     int v1, int v2, int v3, int v4);

/* Print a long value using a format. */
const char *pprintld1(stream * s, const char *format, long v);
const char *pprintld2(stream * s, const char *format, long v1, long v2);
const char *pprintld3(stream * s, const char *format,
		      long v1, long v2, long v3);

/* Print (a) string(s) using a format. */
const char *pprints1(stream * s, const char *format, const char *str);
const char *pprints2(stream * s, const char *format,
		     const char *str1, const char *str2);
const char *pprints3(stream * s, const char *format,
		     const char *str1, const char *str2, const char *str3);

#endif /* spprint_INCLUDED */
