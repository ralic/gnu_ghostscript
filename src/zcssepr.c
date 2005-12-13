/* Copyright (C) 1994, 2000 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: zcssepr.c,v 1.5 2005/12/13 16:57:28 jemarch Exp $ */
/* Separation color space support */
#include "memory_.h"
#include "ghost.h"
#include "oper.h"
#include "gsstruct.h"
#include "gscolor.h"
#include "gsmatrix.h"		/* for gxcolor2.h */
#include "gxcspace.h"
#include "gxfixed.h"		/* ditto */
#include "gxcolor2.h"
#include "estack.h"
#include "ialloc.h"
#include "icsmap.h"
#include "ifunc.h"
#include "igstate.h"
#include "iname.h"
#include "ivmspace.h"
#include "store.h"
#include "gscsepr.h"
#include "gscdevn.h"
#include "gxcdevn.h"
#include "zht2.h"

/* Imported from gscsepr.c */
extern const gs_color_space_type gs_color_space_type_Separation;
/* Imported from gscdevn.c */
extern const gs_color_space_type gs_color_space_type_DeviceN;

/*
 * Adobe first created the separation colorspace type and then later created
 * the DeviceN colorspace.  Logically the separation colorspace is the same
 * as a DeviceN colorspace with a single component, except for the /None and
 * /All parameter values.  We treat the separation colorspace as a DeviceN
 * colorspace except for the /All case.
 */

/* <array> .setseparationspace - */
/* The current color space is the alternate space for the separation space. */
private int
zsetseparationspace(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    const ref *pcsa;
    gs_color_space cs;
    const gs_color_space * pacs;
    ref_colorspace cspace_old;
    ref sname, name_none, name_all;
    gs_device_n_map *pmap = NULL;
    gs_function_t *pfn = NULL;
    separation_type sep_type;
    int code;

    /* Verify that we have an array as our input parameter */
    check_read_type(*op, t_array);
    if (r_size(op) != 4)
	return_error(e_rangecheck);

    /* The alternate color space has been selected as the current color space */
    pacs = gs_currentcolorspace(igs);
    cs = *pacs;
    if (!cs.type->can_be_alt_space)
	return_error(e_rangecheck);

    /*
     * pcsa is a pointer to element 1 (2nd element)  in the Separation colorspace
     * description array.  Thus pcsa[2] is element #3 (4th element) which is the
     * tint transform.
     */
    pcsa = op->value.const_refs + 1;
    sname = *pcsa;
    switch (r_type(&sname)) {
	default:
	    return_error(e_typecheck);
	case t_string:
	    code = name_from_string(&sname, &sname);
	    if (code < 0)
		return code;
	    /* falls through */
	case t_name:
	    break;
    }

    if ((code = name_ref((const byte *)"All", 3, &name_all, 0)) < 0)
	return code;
    if ((code = name_ref((const byte *)"None", 4, &name_none, 0)) < 0)
	return code;
    sep_type = ( name_eq(&sname, &name_all) ? SEP_ALL :
	         name_eq(&sname, &name_none) ? SEP_NONE : SEP_OTHER);

    /* Check tint transform procedure. */
    /* See comment above about psca */
    check_proc(pcsa[2]);
    pfn = ref_function(pcsa + 2);
    if (pfn == NULL)
	return_error(e_rangecheck);

    cspace_old = istate->colorspace;
    /* See zcsindex.c for why we use memmove here. */
    memmove(&cs.params.separation.alt_space, &cs,
	        sizeof(cs.params.separation.alt_space));
    /* Now set the current color space as Separation */
    code = gs_build_Separation(&cs, pacs, imemory);
    if (code < 0)
	return code;
    pmap = cs.params.separation.map;
    gs_cspace_init(&cs, &gs_color_space_type_Separation, NULL);
    cs.params.separation.sep_type = sep_type;
    cs.params.separation.sep_name = name_index(&sname);
    cs.params.separation.get_colorname_string = gs_get_colorname_string;
    istate->colorspace.procs.special.separation.layer_name = pcsa[0];
    istate->colorspace.procs.special.separation.tint_transform = pcsa[2];
    if (code >= 0)
        code = gs_cspace_set_sepr_function(&cs, pfn);
    if (code >= 0)
	code = gs_setcolorspace(igs, &cs);
    if (code < 0) {
	istate->colorspace = cspace_old;
	ifree_object(pmap, ".setseparationspace(pmap)");
	return code;
    }
    rc_decrement(pmap, ".setseparationspace(pmap)");  /* build sets rc = 1 */
    pop(1);
    return 0;
}

/* - currentoverprint <bool> */
private int
zcurrentoverprint(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    push(1);
    make_bool(op, gs_currentoverprint(igs));
    return 0;
}

/* <bool> setoverprint - */
private int
zsetoverprint(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    check_type(*op, t_boolean);
    gs_setoverprint(igs, op->value.boolval);
    pop(1);
    return 0;
}

/* - .currentoverprintmode <int> */
private int
zcurrentoverprintmode(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    push(1);
    make_int(op, gs_currentoverprintmode(igs));
    return 0;
}

/* <int> .setoverprintmode - */
private int
zsetoverprintmode(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int param;
    int code = int_param(op, max_int, &param);

    if (code < 0 || (code = gs_setoverprintmode(igs, param)) < 0)
	return code;
    pop(1);
    return 0;
}

/* ------ Initialization procedure ------ */

const op_def zcssepr_l2_op_defs[] =
{
    op_def_begin_level2(),
    {"0currentoverprint", zcurrentoverprint},
    {"0.currentoverprintmode", zcurrentoverprintmode},
    {"1setoverprint", zsetoverprint},
    {"1.setoverprintmode", zsetoverprintmode},
    {"1.setseparationspace", zsetseparationspace},
    op_def_end(0)
};
