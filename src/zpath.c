/* Copyright (C) 1989, 1995, 1996, 1997, 1998, 1999 artofcode LLC.  All rights reserved.
  
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

/*$Id: zpath.c,v 1.1 2004/01/14 16:59:53 atai Exp $ */
/* Basic path operators */
#include "math_.h"
#include "ghost.h"
#include "oper.h"
#include "igstate.h"
#include "gsmatrix.h"
#include "gspath.h"
#include "store.h"

/* Forward references */
private int common_to(P2(i_ctx_t *,
			 int (*)(P3(gs_state *, floatp, floatp))));
private int common_curve(P2(i_ctx_t *,
  int (*)(P7(gs_state *, floatp, floatp, floatp, floatp, floatp, floatp))));

/* - newpath - */
private int
znewpath(i_ctx_t *i_ctx_p)
{
    return gs_newpath(igs);
}

/* - currentpoint <x> <y> */
private int
zcurrentpoint(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    gs_point pt;
    int code = gs_currentpoint(igs, &pt);

    if (code < 0)
	return code;
    push(2);
    make_real(op - 1, pt.x);
    make_real(op, pt.y);
    return 0;
}

/* <x> <y> moveto - */
int
zmoveto(i_ctx_t *i_ctx_p)
{
    return common_to(i_ctx_p, gs_moveto);
}

/* <dx> <dy> rmoveto - */
int
zrmoveto(i_ctx_t *i_ctx_p)
{
    return common_to(i_ctx_p, gs_rmoveto);
}

/* <x> <y> lineto - */
int
zlineto(i_ctx_t *i_ctx_p)
{
    return common_to(i_ctx_p, gs_lineto);
}

/* <dx> <dy> rlineto - */
int
zrlineto(i_ctx_t *i_ctx_p)
{
    return common_to(i_ctx_p, gs_rlineto);
}

/* Common code for [r](move/line)to */
private int
common_to(i_ctx_t *i_ctx_p,
	  int (*add_proc)(P3(gs_state *, floatp, floatp)))
{
    os_ptr op = osp;
    double opxy[2];
    int code;

    if ((code = num_params(op, 2, opxy)) < 0 ||
	(code = (*add_proc)(igs, opxy[0], opxy[1])) < 0
	)
	return code;
    pop(2);
    return 0;
}

/* <x1> <y1> <x2> <y2> <x3> <y3> curveto - */
int
zcurveto(i_ctx_t *i_ctx_p)
{
    return common_curve(i_ctx_p, gs_curveto);
}

/* <dx1> <dy1> <dx2> <dy2> <dx3> <dy3> rcurveto - */
int
zrcurveto(i_ctx_t *i_ctx_p)
{
    return common_curve(i_ctx_p, gs_rcurveto);
}

/* Common code for [r]curveto */
private int
common_curve(i_ctx_t *i_ctx_p,
	     int (*add_proc)(P7(gs_state *, floatp, floatp, floatp, floatp, floatp, floatp)))
{
    os_ptr op = osp;
    double opxy[6];
    int code;

    if ((code = num_params(op, 6, opxy)) < 0)
	return code;
    code = (*add_proc)(igs, opxy[0], opxy[1], opxy[2], opxy[3], opxy[4], opxy[5]);
    if (code >= 0)
	pop(6);
    return code;
}

/* - closepath - */
int
zclosepath(i_ctx_t *i_ctx_p)
{
    return gs_closepath(igs);
}

/* - initclip - */
private int
zinitclip(i_ctx_t *i_ctx_p)
{
    return gs_initclip(igs);
}

/* - clip - */
private int
zclip(i_ctx_t *i_ctx_p)
{
    return gs_clip(igs);
}

/* - eoclip - */
private int
zeoclip(i_ctx_t *i_ctx_p)
{
    return gs_eoclip(igs);
}

/* ------ Initialization procedure ------ */

const op_def zpath_op_defs[] =
{
    {"0clip", zclip},
    {"0closepath", zclosepath},
    {"0currentpoint", zcurrentpoint},
    {"6curveto", zcurveto},
    {"0eoclip", zeoclip},
    {"0initclip", zinitclip},
    {"2lineto", zlineto},
    {"2moveto", zmoveto},
    {"0newpath", znewpath},
    {"6rcurveto", zrcurveto},
    {"2rlineto", zrlineto},
    {"2rmoveto", zrmoveto},
    op_def_end(0)
};
