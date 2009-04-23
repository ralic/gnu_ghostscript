/* Copyright (C) 2001-2006 Artifex Software, Inc.
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

/* $Id: sfilter1.c,v 1.1 2009/04/23 23:31:54 Arabidopsis Exp $ */
/* Filters included in Level 1 systems: NullEncode/Decode, PFBDecode, */
/*   SubFileDecode. */
#include "stdio_.h"		/* includes std.h */
#include "memory_.h"
#include "strimpl.h"
#include "sfilter.h"

/* ------ PFBDecode ------ */

private_st_PFBD_state();

/* Initialize the state */
static int
s_PFBD_init(stream_state * st)
{
    stream_PFBD_state *const ss = (stream_PFBD_state *) st;

    ss->record_type = -1;
    return 0;
}

/* Process a buffer */
static int
s_PFBD_process(stream_state * st, stream_cursor_read * pr,
	       stream_cursor_write * pw, bool last)
{
    stream_PFBD_state *const ss = (stream_PFBD_state *) st;
    register const byte *p = pr->ptr;
    register byte *q = pw->ptr;
    int rcount, wcount;
    int c;
    int status = 0;

top:
    rcount = pr->limit - p;
    wcount = pw->limit - q;
    switch (ss->record_type) {
	case -1:		/* new record */
	    if (rcount < 2)
		goto out;
	    if (p[1] != 0x80)
		goto err;
	    c = p[2];
	    switch (c) {
		case 1:
		case 2:
		    break;
		case 3:
		    status = EOFC;
		    p += 2;
		    goto out;
		default:
		    p += 2;
		    goto err;
	    }
	    if (rcount < 6)
		goto out;
	    ss->record_type = c;
	    ss->record_left = p[3] + ((uint) p[4] << 8) +
		((ulong) p[5] << 16) +
		((ulong) p[6] << 24);

            /* Check for an invalid counter found in an Adobe font, bug 689617 */
            if (ss->record_left == 0 && ss->record_type == 1) {
               if (p + 7 < pr->limit) {
                   if (p[6] ==  128)
                     ; /* normal empty block */
                   else {
                     ss->record_type = 4; /* ASCII stuff between blocks */
                     ss->record_left = ~0;
                   }
               } else {
                   if (!last)
		      goto out;
               }
            }
	    p += 6;
	    goto top;
	case 1:		/* text data */
	    /* Translate \r to \n. */
	    {
		int count = (wcount < rcount ? (status = 1, wcount) : rcount);

		if (count > ss->record_left)
		    count = ss->record_left,
			status = 0;
		ss->record_left -= count;
		for (; count != 0; count--) {
		    c = *++p;
		    *++q = (c == '\r' ? '\n' : c);
		}
	    }
	    break;
	case 2:		/* binary data */
	    if (ss->binary_to_hex) {
		/* Translate binary to hex. */
		int count;
		const char *const hex_digits = "0123456789abcdef";

		wcount >>= 1;	/* 2 chars per input byte */
		count = (wcount < rcount ? (status = 1, wcount) : rcount);
		if (count > ss->record_left)
		    count = ss->record_left,
			status = 0;
		ss->record_left -= count;
		for (; count != 0; count--) {
		    c = *++p;
		    q[1] = hex_digits[c >> 4];
		    q[2] = hex_digits[c & 0xf];
		    q += 2;
		}
	    } else {		/* Just read binary data. */
		int count = (wcount < rcount ? (status = 1, wcount) : rcount);

		if (count > ss->record_left)
		    count = ss->record_left,
			status = 0;
		ss->record_left -= count;
		memcpy(q + 1, p + 1, count);
		p += count;
		q += count;
	    }
	    break;
	case 4:
            /* Treat the text after empty ASCII block as ACSII stream */
	    /* Translate \r to \n. */
	    {
		int count = (wcount < rcount ? (status = 1, wcount) : rcount);
		for (; count != 0; count--) {
		    c = *++p;
                    if (c == 128)
                      { --p;
                        ss->record_left = 0;
                        break;
                      }
		    *++q = (c == '\r' ? '\n' : c);
		}
	    }
	    break;
    }
    if (ss->record_left == 0) {
	ss->record_type = -1;
	goto top;
    }
out:
    pr->ptr = p;
    pw->ptr = q;
    return status;
err:
    pr->ptr = p;
    pw->ptr = q;
    return ERRC;
}

/* Stream template */
const stream_template s_PFBD_template = {
    &st_PFBD_state, s_PFBD_init, s_PFBD_process, 6, 2
};

/* ------ SubFileDecode ------ */

private_st_SFD_state();

/* Set default parameter values. */
static void
s_SFD_set_defaults(stream_state * st)
{
    stream_SFD_state *const ss = (stream_SFD_state *) st;

    ss->count = 0;
    ss->eod.data = 0;
    ss->eod.size = 0;
    ss->skip_count = 0;
}

/* Initialize the stream */
static int
s_SFD_init(stream_state * st)
{
    stream_SFD_state *const ss = (stream_SFD_state *) st;

    ss->match = 0;
    ss->copy_count = 0;
    ss->min_left = (ss->eod.size != 0);

    return 0;
}

/* Refill the buffer */
static int
s_SFD_process(stream_state * st, stream_cursor_read * pr,
	      stream_cursor_write * pw, bool last)
{
    stream_SFD_state *const ss = (stream_SFD_state *) st;
    register const byte *p = pr->ptr;
    register byte *q = pw->ptr;
    const byte *rlimit = pr->limit;
    byte *wlimit = pw->limit;
    int status = 0;

    if (ss->eod.size == 0) {	/* Just read, with no EOD pattern. */
	int rcount = rlimit - p;
	int wcount = wlimit - q;
	int count;

	if (rcount <= ss->skip_count) { /* skipping */
	    ss->skip_count -= rcount;
	    pr->ptr = rlimit;
	    return 0;
	} else if (ss->skip_count > 0) {
	    rcount -= ss->skip_count;
	    pr->ptr = p += ss->skip_count;
	    ss->skip_count = 0;
	}
	count = min(rcount, wcount);
	if (ss->count == 0)	/* no EOD limit */
	    return stream_move(pr, pw);
	else if (ss->count > count) {	/* not EOD yet */
	    ss->count -= count;
	    return stream_move(pr, pw);
	} else {		/* We're going to reach EOD. */
	    count = ss->count;
	    if (count > 0) {
		memcpy(q + 1, p + 1, count);
		pr->ptr = p + count;
		pw->ptr = q + count;
	    }
	    ss->count = -1;
	    return EOFC;
	}
    } else {			/* Read looking for an EOD pattern. */
	const byte *pattern = ss->eod.data;
	uint match = ss->match;

cp:
	/* Check whether we're still copying a partial match. */
	if (ss->copy_count) {
	    int count = min(wlimit - q, ss->copy_count);

	    memcpy(q + 1, ss->eod.data + ss->copy_ptr, count);
	    ss->copy_count -= count;
	    ss->copy_ptr += count;
	    q += count;
	    if (ss->copy_count != 0) {	/* hit wlimit */
		status = 1;
		goto xit;
	    } else if (ss->count < 0) {
		status = EOFC;
		goto xit;
	    }
	}
	while (p < rlimit) {
	    int c = *++p;

	    if (c == pattern[match]) {
		if (++match == ss->eod.size) {
		    if (ss->skip_count > 0) {
			q = pw->ptr; /* undo any writes */
			ss->skip_count--;
			match = 0;
			continue;
		    }
		    /*
		     * We use if/else rather than switch because the value
		     * is long, which is not supported as a switch value in
		     * pre-ANSI C.
		     */
		    if (ss->count <= 0) {
			status = EOFC;
			goto xit;
		    } else if (ss->count == 1) {
			ss->count = -1;
		    } else
			ss->count--;
		    ss->copy_ptr = 0;
		    ss->copy_count = match;
		    match = 0;
		    goto cp;
		}
		continue;
	    }
	    /*
	     * No match here, back up to find the longest one.
	     * This may be quadratic in string_size, but
	     * we don't expect this to be a real problem.
	     */
	    if (match > 0) {
		int end = match;

		while (match > 0) {
		    match--;
		    if (!memcmp(pattern, pattern + end - match, match))
			break;
		}
		/*
		 * Copy the unmatched initial portion of
		 * the EOD string to the output.
		 */
		p--;
		ss->copy_ptr = 0;
		ss->copy_count = end - match;
		goto cp;
	    }
	    if (q == wlimit) {
		p--;
		status = 1;
		break;
	    }
	    *++q = c;
	}
xit:	pr->ptr = p;
	if (ss->skip_count <= 0)
	    pw->ptr = q;
	ss->match = match;
    }
    return status;
}

/* Stream template */
const stream_template s_SFD_template = {
    &st_SFD_state, s_SFD_init, s_SFD_process, 1, 1, 0, s_SFD_set_defaults
};
