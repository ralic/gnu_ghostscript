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

/* $Id: smd5.c,v 1.9 2007/09/11 15:24:10 Arabidopsis Exp $ */
/* MD5Encode filter */
#include "memory_.h"
#include "strimpl.h"
#include "stream.h"
#include "smd5.h"

/* ------ MD5Encode ------ */

private_st_MD5E_state();

/* Initialize the state. */
private int
s_MD5E_init(stream_state * st)
{
    stream_MD5E_state *const ss = (stream_MD5E_state *) st;

    gs_md5_init(&ss->md5);
    return 0;
}

/* Process a buffer. */
private int
s_MD5E_process(stream_state * st, stream_cursor_read * pr,
	       stream_cursor_write * pw, bool last)
{
    stream_MD5E_state *const ss = (stream_MD5E_state *) st;
    int status = 0;

    if (pr->ptr < pr->limit) {
	gs_md5_append(&ss->md5, pr->ptr + 1, pr->limit - pr->ptr);
	pr->ptr = pr->limit;
    }
    if (last) {
	if (pw->limit - pw->ptr >= 16) {
	    gs_md5_finish(&ss->md5, pw->ptr + 1);
	    pw->ptr += 16;
	    status = EOFC;
	} else
	    status = 1;
    }
    return status;
}

/* Stream template */
const stream_template s_MD5E_template = {
    &st_MD5E_state, s_MD5E_init, s_MD5E_process, 1, 16
};

stream *
s_MD5E_make_stream(gs_memory_t *mem, byte *digest, int digest_size)
{
    stream *s = s_alloc(mem, "s_MD5E_make_stream");
    stream_state *ss = s_alloc_state(mem, s_MD5E_template.stype, "s_MD5E_make_stream");

    if (ss == NULL || s == NULL)
	goto err;
    ss->template = &s_MD5E_template;
    if (s_init_filter(s, ss, digest, digest_size, NULL) < 0)
goto err;
    s->strm = s;
    return s;
err:
    gs_free_object(mem, ss, "s_MD5E_make_stream");
    gs_free_object(mem, s, "s_MD5E_make_stream");
    return NULL;
}

/* Process a buffer. */
private int
s_MD5C_process(stream_state * st, stream_cursor_read * pr,
	       stream_cursor_write * pw, bool last)
{
    stream_MD5E_state *const ss = (stream_MD5E_state *) st;
    int nr = pr->limit - pr->ptr;
    int nw = pw->limit - pw->ptr;
    int n = min(nr, nw);

    gs_md5_append(&ss->md5, pr->ptr + 1, n);
    memcpy(pw->ptr + 1, pr->ptr + 1, n);
    pr->ptr += n;
    pw->ptr += n;
    if (pw->limit == pw->ptr)
	return 1;
    return 0;
}
/* Stream template */
const stream_template s_MD5C_template = {
    &st_MD5E_state, s_MD5E_init, s_MD5C_process, 1, 1024
};

stream *
s_MD5C_make_stream(gs_memory_t *mem, stream *strm)
{
    stream *s = s_alloc(mem, "s_MD5E_make_stream");
    stream_state *ss = s_alloc_state(mem, s_MD5E_template.stype, "s_MD5E_make_stream");
    int buffer_size = 1024;
    byte *buffer = gs_alloc_bytes(mem, buffer_size, "s_MD5E_make_stream(buffer)");

    if (ss == NULL || s == NULL || buffer == NULL)
	goto err;
    ss->template = &s_MD5C_template;
    if (s_init_filter(s, ss, buffer, buffer_size, NULL) < 0)
	goto err;
    s->strm = strm;
    s->close_strm = true;
    return s;
err:
    gs_free_object(mem, ss, "s_MD5E_make_stream");
    gs_free_object(mem, s, "s_MD5E_make_stream");
    gs_free_object(mem, buffer, "s_MD5E_make_stream");
    return NULL;
}

/* Get a digest. Apply 'flush' before calling. */
int
s_MD5C_get_digest(stream *s, byte *buf, int buf_length)
{
    stream_MD5E_state *const ss = (stream_MD5E_state *)s->state;
    gs_md5_state_t md5;
    byte b[16], *p;
    int l = min(16, buf_length), k;

    if (s->procs.process != s_MD5C_process)
	return 0; /* Must not happen. */
    md5 = ss->md5;
    gs_md5_finish(&md5, b);
    memcpy(buf, b, l);
    for (p = b + l; p < b + sizeof(b); p += l) {
	for (k = 0; k < l && p + k < b + sizeof(b); k++)
	    buf[k] ^= p[k];
    }
    return l;
}


