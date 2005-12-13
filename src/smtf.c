/* Copyright (C) 1995, 1996, 1998 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: smtf.c,v 1.4 2005/12/13 16:57:28 jemarch Exp $ */
/* MoveToFront filters */
#include "stdio_.h"
#include "strimpl.h"
#include "smtf.h"

/* ------ MoveToFrontEncode/Decode ------ */

private_st_MTF_state();

/* Initialize */
private int
s_MTF_init(stream_state * st)
{
    stream_MTF_state *const ss = (stream_MTF_state *) st;
    int i;

    for (i = 0; i < 256; i++)
	ss->prev.b[i] = (byte) i;
    return 0;
}

/* Encode a buffer */
private int
s_MTFE_process(stream_state * st, stream_cursor_read * pr,
	       stream_cursor_write * pw, bool last)
{
    stream_MTF_state *const ss = (stream_MTF_state *) st;
    register const byte *p = pr->ptr;
    register byte *q = pw->ptr;
    const byte *rlimit = pr->limit;
    uint count = rlimit - p;
    uint wcount = pw->limit - q;
    int status =
	(count < wcount ? 0 : (rlimit = p + wcount, 1));

    while (p < rlimit) {
	byte b = *++p;
	int i;
	byte prev = b, repl;

	for (i = 0; (repl = ss->prev.b[i]) != b; i++)
	    ss->prev.b[i] = prev, prev = repl;
	ss->prev.b[i] = prev;
	*++q = (byte) i;
    }
    pr->ptr = p;
    pw->ptr = q;
    return status;
}

/* Stream template */
const stream_template s_MTFE_template = {
    &st_MTF_state, s_MTF_init, s_MTFE_process, 1, 1
};

/* Decode a buffer */
private int
s_MTFD_process(stream_state * st, stream_cursor_read * pr,
	       stream_cursor_write * pw, bool last)
{
    stream_MTF_state *const ss = (stream_MTF_state *) st;
    register const byte *p = pr->ptr;
    register byte *q = pw->ptr;
    const byte *rlimit = pr->limit;
    uint count = rlimit - p;
    uint wcount = pw->limit - q;
    int status = (count <= wcount ? 0 : (rlimit = p + wcount, 1));

    /* Cache the first few entries in local variables. */
    byte
	v0 = ss->prev.b[0], v1 = ss->prev.b[1],
	v2 = ss->prev.b[2], v3 = ss->prev.b[3];

    while (p < rlimit) {
	byte first;

	/* Zeros far outnumber all other bytes in the BWBS */
	/* code; check for them first. */
	if (*++p == 0) {
	    *++q = v0;
	    continue;
	}
	switch (*p) {
	    default:
		{
		    uint b = *p;
		    byte *bp = &ss->prev.b[b];

		    *++q = first = *bp;
#if arch_sizeof_long == 4
		    ss->prev.b[3] = v3;
#endif
		    /* Move trailing entries individually. */
		    for (;; bp--, b--) {
			*bp = bp[-1];
			if (!(b & (sizeof(long) - 1)))
			         break;
		    }
		    /* Move in long-size chunks. */
		    for (; (b -= sizeof(long)) != 0;) {
			bp -= sizeof(long);

#if arch_is_big_endian
			*(ulong *) bp =
			    (*(ulong *) bp >> 8) |
			    ((ulong) bp[-1] << ((sizeof(long) - 1) * 8));

#else
			*(ulong *) bp = (*(ulong *) bp << 8) | bp[-1];
#endif
		    }
		}
#if arch_sizeof_long > 4	/* better be 8! */
		goto m7;
	    case 7:
		*++q = first = ss->prev.b[7];
m7:		ss->prev.b[7] = ss->prev.b[6];
		goto m6;
	    case 6:
		*++q = first = ss->prev.b[6];
m6:		ss->prev.b[6] = ss->prev.b[5];
		goto m5;
	    case 5:
		*++q = first = ss->prev.b[5];
m5:		ss->prev.b[5] = ss->prev.b[4];
		goto m4;
	    case 4:
		*++q = first = ss->prev.b[4];
m4:		ss->prev.b[4] = v3;
#endif
		goto m3;
	    case 3:
		*++q = first = v3;
m3:		v3 = v2, v2 = v1, v1 = v0, v0 = first;
		break;
	    case 2:
		*++q = first = v2;
		v2 = v1, v1 = v0, v0 = first;
		break;
	    case 1:
		*++q = first = v1;
		v1 = v0, v0 = first;
		break;
	}
    }
    ss->prev.b[0] = v0;
    ss->prev.b[1] = v1;
    ss->prev.b[2] = v2;
    ss->prev.b[3] = v3;
    pr->ptr = p;
    pw->ptr = q;
    return status;
}

/* Stream template */
const stream_template s_MTFD_template = {
    &st_MTF_state, s_MTF_init, s_MTFD_process, 1, 1,
    NULL, NULL, s_MTF_init
};
