/* Copyright (C) 1990, 1995, 1996 artofcode LLC.  All rights reserved.
  
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

/*$Id: scanchar.h,v 1.1 2004/01/14 16:59:52 atai Exp $ */
/* Definitions for token scanner character type table */
/* Requires scommon.h */

#ifndef scanchar_INCLUDED
#  define scanchar_INCLUDED

/*
 * An array for fast scanning of names, numbers, and hex strings.
 * Indexed by character code (including exceptions), it contains:
 *      0 - max_radix-1 for valid digits,
 *      ctype_name for other characters valid in names,
 *      ctype_btoken for characters introducing binary tokens
 *        (if the binary token feature is enabled),
 *      ctype_space for whitespace characters,
 *      ctype_exception for exceptions (see scommon.h), and
 *      ctype_other for everything else.
 * Exceptions are negative values; we bias the table origin accordingly.
 *
 * NOTE: This table is defined in iscantab.c and used in a variety of places.
 * If any of the values below change, you must edit the table.
 */
extern const byte scan_char_array[max_stream_exception + 256];

#define scan_char_decoder (&scan_char_array[max_stream_exception])
#define min_radix 2
#define max_radix 36
#define ctype_name 100
#define ctype_btoken 101
#define ctype_space 102
#define ctype_other 103
#define ctype_exception 104
/* Special characters with no \xxx representation */
#define char_NULL 0
#define char_EOT 004		/* ^D, job delimiter */
#define char_VT 013		/* ^K, vertical tab */
#define char_DOS_EOF 032	/* ^Z */
/*
 * Most systems define '\n' as 0x0a and '\r' as 0x0d; however, OS-9
 * has '\n' = '\r' = 0x0d and '\l' = 0x0a.  To deal with this,
 * we introduce abstract characters char_CR and char_EOL such that
 * any of [char_CR], [char_CR char_EOL], or [char_EOL] is recognized
 * as an end-of-line sequence.
 */
#define char_CR '\r'
#if '\r' == '\n'
#  define char_EOL 0x0a		/* non-OS-9 compilers complain about '\l' */
#else
#  define char_EOL '\n'
#endif

#endif /* scanchar_INCLUDED */
