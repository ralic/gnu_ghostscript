/* Copyright (C) 1994, 2000 artofcode LLC.  All rights reserved.
  
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

/*$Id: zfdctd.c,v 1.1 2004/01/14 16:59:53 atai Exp $ */
/* DCTDecode filter creation */
#include "memory_.h"
#include "stdio_.h"		/* for jpeglib.h */
#include "jpeglib_.h"
#include "ghost.h"
#include "oper.h"
#include "gsmalloc.h"		/* for gs_memory_default */
#include "strimpl.h"
#include "sdct.h"
#include "sjpeg.h"
#include "ialloc.h"
#include "ifilter.h"
#include "iparam.h"

private_st_jpeg_decompress_data();

/* Import the parameter processing procedure from sddparam.c */
stream_state_proc_put_params(s_DCTD_put_params, stream_DCT_state);

/* <source> <dict> DCTDecode/filter <file> */
/* <source> DCTDecode/filter <file> */
private int
zDCTD(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    gs_memory_t *mem = (gs_memory_t *)(i_ctx_p->memory.current);
    stream_DCT_state state;
    dict_param_list list;
    jpeg_decompress_data *jddp;
    int code;
    const ref *dop;
    uint dspace;

    /* First allocate space for IJG parameters. */
    jddp = gs_alloc_struct_immovable(mem,jpeg_decompress_data,
      &st_jpeg_decompress_data, "zDCTD");
    if (jddp == 0)
	return_error(e_VMerror);
    if (s_DCTD_template.set_defaults)
	(*s_DCTD_template.set_defaults) ((stream_state *) & state);
    state.data.decompress = jddp;
    jddp->memory = state.jpeg_memory = mem;	/* set now for allocation */
    jddp->scanline_buffer = NULL;	/* set this early for safe error exit */
    state.report_error = filter_report_error;	/* in case create fails */
    if ((code = gs_jpeg_create_decompress(&state)) < 0)
	goto fail;		/* correct to do jpeg_destroy here */
    /* Read parameters from dictionary */
    if (r_has_type(op, t_dictionary))
	dop = op, dspace = r_space(op);
    else
	dop = 0, dspace = 0;
    if ((code = dict_param_list_read(&list, dop, NULL, false, iimemory)) < 0)
	goto fail;
    if ((code = s_DCTD_put_params((gs_param_list *) & list, &state)) < 0)
	goto rel;
    /* Create the filter. */
    jddp->template = s_DCTD_template;
    code = filter_read(i_ctx_p, 0, &jddp->template,
		       (stream_state *) & state, dspace);
    if (code >= 0)		/* Success! */
	return code;
    /*
     * We assume that if filter_read fails, the stream has not been
     * registered for closing, so s_DCTD_release will never be called.
     * Therefore we free the allocated memory before failing.
     */
rel:
    iparam_list_release(&list);
fail:
    gs_jpeg_destroy(&state);
    gs_free_object(mem, jddp, "zDCTD fail");
    return code;
}

/* ------ Initialization procedure ------ */

const op_def zfdctd_op_defs[] =
{
    op_def_begin_filter(),
    {"2DCTDecode", zDCTD},
    op_def_end(0)
};
