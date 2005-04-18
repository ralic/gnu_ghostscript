/* Copyright (C) 1997, 1998 Aladdin Enterprises.  All rights reserved.
  
  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License version 2
  as published by the Free Software Foundation.


  This software is provided AS-IS with no warranty, either express or
  implied. That is, this program is distributed in the hope that it will 
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place, Suite 330, Boston, MA, 02111-1307.
  
  For more information about licensing, please refer to
  http://www.ghostscript.com/licensing/. For information on
  commercial licensing, go to http://www.artifex.com/licensing/ or
  contact Artifex Software, Inc., 101 Lucas Valley Road #110,
  San Rafael, CA  94903, U.S.A., +1(415)492-9861.
*/

/* $Id: gxpflat.c,v 1.3 2005/04/18 12:06:05 Arabidopsis Exp $ */
/* Path flattening algorithms */
#include "gx.h"
#include "gxarith.h"
#include "gxfixed.h"
#include "gzpath.h"
#include "vdtrace.h"

/* Define whether to merge nearly collinear line segments when flattening */
/* curves.  This is very good for performance, but we feel a little */
/* uneasy about its effects on character appearance. */
#define MERGE_COLLINEAR_SEGMENTS 1

/* ---------------- Curve flattening ---------------- */

#define x1 pc->p1.x
#define y1 pc->p1.y
#define x2 pc->p2.x
#define y2 pc->p2.y
#define x3 pc->pt.x
#define y3 pc->pt.y

/*
 * To calculate how many points to sample along a path in order to
 * approximate it to the desired degree of flatness, we define
 *      dist((x,y)) = abs(x) + abs(y);
 * then the number of points we need is
 *      N = 1 + sqrt(3/4 * D / flatness),
 * where
 *      D = max(dist(p0 - 2*p1 + p2), dist(p1 - 2*p2 + p3)).
 * Since we are going to use a power of 2 for the number of intervals,
 * we can avoid the square root by letting
 *      N = 1 + 2^(ceiling(log2(3/4 * D / flatness) / 2)).
 * (Reference: DEC Paris Research Laboratory report #1, May 1989.)
 *
 * We treat two cases specially.  First, if the curve is very
 * short, we halve the flatness, to avoid turning short shallow curves
 * into short straight lines.  Second, if the curve forms part of a
 * character (indicated by flatness = 0), we let
 *      N = 1 + 2 * max(abs(x3-x0), abs(y3-y0)).
 * This is probably too conservative, but it produces good results.
 */
int
gx_curve_log2_samples(fixed x0, fixed y0, const curve_segment * pc,
		      fixed fixed_flat)
{
    fixed
	x03 = x3 - x0,
	y03 = y3 - y0;
    int k;

    if (x03 < 0)
	x03 = -x03;
    if (y03 < 0)
	y03 = -y03;
    if ((x03 | y03) < int2fixed(16))
	fixed_flat >>= 1;
    if (fixed_flat == 0) {	/* Use the conservative method. */
	fixed m = max(x03, y03);

	for (k = 1; m > fixed_1;)
	    k++, m >>= 1;
    } else {
	const fixed
	      x12 = x1 - x2, y12 = y1 - y2, dx0 = x0 - x1 - x12, dy0 = y0 - y1 - y12,
	      dx1 = x12 - x2 + x3, dy1 = y12 - y2 + y3, adx0 = any_abs(dx0),
	      ady0 = any_abs(dy0), adx1 = any_abs(dx1), ady1 = any_abs(dy1);

	fixed
	    d = max(adx0, adx1) + max(ady0, ady1);
	/*
	 * The following statement is split up to work around a
	 * bug in the gcc 2.7.2 optimizer on H-P RISC systems.
	 */
	uint qtmp = d - (d >> 2) /* 3/4 * D */ +fixed_flat - 1;
	uint q = qtmp / fixed_flat;

	if_debug6('2', "[2]d01=%g,%g d12=%g,%g d23=%g,%g\n",
		  fixed2float(x1 - x0), fixed2float(y1 - y0),
		  fixed2float(-x12), fixed2float(-y12),
		  fixed2float(x3 - x2), fixed2float(y3 - y2));
	if_debug2('2', "     D=%f, flat=%f,",
		  fixed2float(d), fixed2float(fixed_flat));
	/* Now we want to set k = ceiling(log2(q) / 2). */
	for (k = 0; q > 1;)
	    k++, q = (q + 3) >> 2;
	if_debug1('2', " k=%d\n", k);
    }
    return k;
}

/*
 * Define the maximum number of points for sampling if we want accurate
 * rasterizing.  2^(k_sample_max*3)-1 must fit into a uint with a bit
 * to spare; also, we must be able to compute 1/2^(3*k) by table lookup.
 */
#define k_sample_max min((size_of(int) * 8 - 1) / 3, 10)

/*
 * Split a curve segment into two pieces at the (parametric) midpoint.
 * Algorithm is from "The Beta2-split: A special case of the Beta-spline
 * Curve and Surface Representation," B. A. Barsky and A. D. DeRose, IEEE,
 * 1985, courtesy of Crispin Goswell.
 */
private void
split_curve_midpoint(fixed x0, fixed y0, const curve_segment * pc,
		     curve_segment * pc1, curve_segment * pc2)
{				/*
				 * We have to define midpoint carefully to avoid overflow.
				 * (If it overflows, something really pathological is going
				 * on, but we could get infinite recursion that way....)
				 */
#define midpoint(a,b)\
    (arith_rshift_1(a) + arith_rshift_1(b) + ((a) & (b) & 1) + 1)
    fixed x12 = midpoint(x1, x2);
    fixed y12 = midpoint(y1, y2);

    /*
     * pc1 or pc2 may be the same as pc, so we must be a little careful
     * about the order in which we store the results.
     */
    pc1->p1.x = midpoint(x0, x1);
    pc1->p1.y = midpoint(y0, y1);
    pc2->p2.x = midpoint(x2, x3);
    pc2->p2.y = midpoint(y2, y3);
    pc1->p2.x = midpoint(pc1->p1.x, x12);
    pc1->p2.y = midpoint(pc1->p1.y, y12);
    pc2->p1.x = midpoint(x12, pc2->p2.x);
    pc2->p1.y = midpoint(y12, pc2->p2.y);
    if (pc2 != pc)
	pc2->pt.x = pc->pt.x,
	    pc2->pt.y = pc->pt.y;
    pc1->pt.x = midpoint(pc1->p2.x, pc2->p1.x);
    pc1->pt.y = midpoint(pc1->p2.y, pc2->p1.y);
#undef midpoint
}

private inline void
print_points(const gs_fixed_point *points, int count)
{
#ifdef DEBUG    
    int i;

    if (!gs_debug_c('3'))
	return;
    for (i = 0; i < count; i++)
	if_debug2('3', "[3]out x=%d y=%d\n", points[i].x, points[i].y);
#endif
}

#if !FLATTENED_CURVE_ITERATOR
/*
 * Flatten a segment of the path by repeated sampling.
 * 2^k is the number of lines to produce (i.e., the number of points - 1,
 * including the endpoints); we require k >= 1.
 * If k or any of the coefficient values are too large,
 * use recursive subdivision to whittle them down.
 */

int
gx_subdivide_curve(gx_path * ppath, int k, curve_segment * pc,
		  segment_notes notes)
{
    fixed x0, y0;

    /* x1 ... y3 were defined above */
    fixed cx, bx, ax, cy, by, ay;
    fixed ptx, pty;
    fixed x, y;

    /*
     * We can compute successive values by finite differences,
     * using the formulas:
     x(t) =
     a*t^3 + b*t^2 + c*t + d =>
     dx(t) = x(t+e)-x(t) =
     a*(3*t^2*e + 3*t*e^2 + e^3) + b*(2*t*e + e^2) + c*e =
     (3*a*e)*t^2 + (3*a*e^2 + 2*b*e)*t + (a*e^3 + b*e^2 + c*e) =>
     d2x(t) = dx(t+e)-dx(t) =
     (3*a*e)*(2*t*e + e^2) + (3*a*e^2 + 2*b*e)*e =
     (6*a*e^2)*t + (6*a*e^3 + 2*b*e^2) =>
     d3x(t) = d2x(t+e)-d2x(t) =
     6*a*e^3;
     x(0) = d, dx(0) = (a*e^3 + b*e^2 + c*e),
     d2x(0) = 6*a*e^3 + 2*b*e^2;
     * In these formulas, e = 1/2^k; of course, there are separate
     * computations for the x and y values.
     *
     * There is a tradeoff in doing the above computation in fixed
     * point.  If we separate out the constant term (d) and require that
     * all the other values fit in a long, then on a 32-bit machine with
     * 12 bits of fraction in a fixed, k = 4 implies a maximum curve
     * size of 128 pixels; anything larger requires subdividing the
     * curve.  On the other hand, doing the computations in explicit
     * double precision slows down the loop by a factor of 3 or so.  We
     * found to our surprise that the latter is actually faster, because
     * the additional subdivisions cost more than the slower loop.
     *
     * We represent each quantity as I+R/M, where I is an "integer" and
     * the "remainder" R lies in the range 0 <= R < M=2^(3*k).  Note
     * that R may temporarily exceed M; for this reason, we require that
     * M have at least one free high-order bit.  To reduce the number of
     * variables, we don't actually compute M, only M-1 (rmask).  */
    uint i;
    uint rmask;			/* M-1 */
    fixed idx, idy, id2x, id2y, id3x, id3y;	/* I */
    uint rx, ry, rdx, rdy, rd2x, rd2y, rd3x, rd3y;	/* R */
    gs_fixed_point *ppt;

#define max_points 50		/* arbitrary */
    gs_fixed_point points[max_points + 1];

  top:x0 = ppath->position.x;
    y0 = ppath->position.y;
#ifdef DEBUG
    if (gs_debug_c('3')) {
	dlprintf4("[3]x0=%f y0=%f x1=%f y1=%f\n",
		  fixed2float(x0), fixed2float(y0),
		  fixed2float(x1), fixed2float(y1));
	dlprintf5("   x2=%f y2=%f x3=%f y3=%f  k=%d\n",
		  fixed2float(x2), fixed2float(y2),
		  fixed2float(x3), fixed2float(y3), k);
    }
#endif
    {
	fixed x01, x12, y01, y12;

	curve_points_to_coefficients(x0, x1, x2, x3, ax, bx, cx,
				     x01, x12);
	curve_points_to_coefficients(y0, y1, y2, y3, ay, by, cy,
				     y01, y12);
    }

    if_debug6('3', "[3]ax=%f bx=%f cx=%f\n   ay=%f by=%f cy=%f\n",
	      fixed2float(ax), fixed2float(bx), fixed2float(cx),
	      fixed2float(ay), fixed2float(by), fixed2float(cy));
#define max_fast (max_fixed / 6)
#define min_fast (-max_fast)
#define in_range(v) (v < max_fast && v > min_fast)
    if (k == 0) {		/* The curve is very short, or anomalous in some way. */
	/* Just add a line and exit. */
	vd_lineto(x3, y3);
	print_points(&pc->pt, 1);
	return gx_path_add_line_notes(ppath, x3, y3, notes);
    }
    if (k <= k_sample_max &&
	in_range(ax) && in_range(ay) &&
	in_range(bx) && in_range(by) &&
	in_range(cx) && in_range(cy)
	) {
	x = x0, y = y0;
	rx = ry = 0;
	ppt = points;
	/* Fast check for N == 3, a common special case */
	/* for small characters. */
	if (k == 1) {
#define poly2(a,b,c)\
  arith_rshift_1(arith_rshift_1(arith_rshift_1(a) + b) + c)
	    x += poly2(ax, bx, cx);
	    y += poly2(ay, by, cy);
#undef poly2
	    if_debug2('3', "[3]dx=%f, dy=%f\n",
		      fixed2float(x - x0), fixed2float(y - y0));
	    if_debug5('3', "[3]%s x=%g, y=%g x=%d y=%d\n",
		      (((x ^ x0) | (y ^ y0)) & float2fixed(-0.5) ?
		       "add" : "skip"),
		      fixed2float(x), fixed2float(y), x, y);
	    if (((x ^ x0) | (y ^ y0)) & float2fixed(-0.5))
		ppt->x = ptx = x,
		    ppt->y = pty = y,
		    ppt++;
	    goto last;
	} else {
	    fixed bx2 = bx << 1, by2 = by << 1;
	    fixed ax6 = ((ax << 1) + ax) << 1, ay6 = ((ay << 1) + ay) << 1;

#define adjust_rem(r, q)\
  if ( r > rmask ) q ++, r &= rmask
	    const int k2 = k << 1;
	    const int k3 = k2 + k;

	    rmask = (1 << k3) - 1;
	    /* We can compute all the remainders as ints, */
	    /* because we know they don't exceed M. */
	    /* cx/y terms */
	    idx = arith_rshift(cx, k),
		idy = arith_rshift(cy, k);
	    rdx = ((uint) cx << k2) & rmask,
		rdy = ((uint) cy << k2) & rmask;
	    /* bx/y terms */
	    id2x = arith_rshift(bx2, k2),
		id2y = arith_rshift(by2, k2);
	    rd2x = ((uint) bx2 << k) & rmask,
		rd2y = ((uint) by2 << k) & rmask;
	    idx += arith_rshift_1(id2x),
		idy += arith_rshift_1(id2y);
	    rdx += ((uint) bx << k) & rmask,
		rdy += ((uint) by << k) & rmask;
	    adjust_rem(rdx, idx);
	    adjust_rem(rdy, idy);
	    /* ax/y terms */
	    idx += arith_rshift(ax, k3),
		idy += arith_rshift(ay, k3);
	    rdx += (uint) ax & rmask,
		rdy += (uint) ay & rmask;
	    adjust_rem(rdx, idx);
	    adjust_rem(rdy, idy);
	    id2x += id3x = arith_rshift(ax6, k3),
		id2y += id3y = arith_rshift(ay6, k3);
	    rd2x += rd3x = (uint) ax6 & rmask,
		rd2y += rd3y = (uint) ay6 & rmask;
	    adjust_rem(rd2x, id2x);
	    adjust_rem(rd2y, id2y);
#undef adjust_rem
	}
    } else {			/*
				 * Curve is too long.  Break into two pieces and recur.
				 */
	curve_segment cseg;
	int code;

	k--;
	split_curve_midpoint(x0, y0, pc, &cseg, pc);
	code = gx_subdivide_curve(ppath, k, &cseg, notes);
	if (code < 0)
	    return code;
	notes |= sn_not_first;
	goto top;
    }
#undef in_range
    if_debug1('2', "[2]sampling k=%d\n", k);
    ptx = x0, pty = y0;
    for (i = (1 << k) - 1;;) {
	int code;

#ifdef DEBUG
	if (gs_debug_c('3')) {
	    dlprintf4("[3]dx=%f+%d, dy=%f+%d\n",
		      fixed2float(idx), rdx,
		      fixed2float(idy), rdy);
	    dlprintf4("   d2x=%f+%d, d2y=%f+%d\n",
		      fixed2float(id2x), rd2x,
		      fixed2float(id2y), rd2y);
	    dlprintf4("   d3x=%f+%d, d3y=%f+%d\n",
		      fixed2float(id3x), rd3x,
		      fixed2float(id3y), rd3y);
	}
#endif
#define accum(i, r, di, dr)\
  if ( (r += dr) > rmask ) r &= rmask, i += di + 1;\
  else i += di
	accum(x, rx, idx, rdx);
	accum(y, ry, idy, rdy);
	if_debug5('3', "[3]%s x=%g, y=%g x=%d y=%d\n",
		  (((x ^ ptx) | (y ^ pty)) & float2fixed(-0.5) ?
		   "add" : "skip"),
		  fixed2float(x), fixed2float(y), x, y);
	/*
	 * Skip very short segments -- those that lie entirely within
	 * a square half-pixel.  Also merge nearly collinear
	 * segments -- those where one coordinate of all three points
	 * (the two endpoints and the midpoint) lie within the same
	 * half-pixel and both coordinates are monotonic.
	 * Note that ptx/y, the midpoint, is the same as ppt[-1].x/y;
	 * the previous point is ppt[-2].x/y.
	 */
#define coord_near(v, ptv)\
  (!( ((v) ^ (ptv)) & float2fixed(-0.5) ))
#define coords_in_order(v0, v1, v2)\
  ( (((v1) - (v0)) ^ ((v2) - (v1))) >= 0 )
	if (coord_near(x, ptx)) {	/* X coordinates are within a half-pixel. */
	    if (coord_near(y, pty))
		goto skip;	/* short segment */
#if MERGE_COLLINEAR_SEGMENTS
	    /* Check for collinear segments. */
	    if (ppt > points + 1 && coord_near(x, ppt[-2].x) &&
		coords_in_order(ppt[-2].x, ptx, x) &&
		coords_in_order(ppt[-2].y, pty, y)
		)
		--ppt;		/* remove middle point */
#endif
	} else if (coord_near(y, pty)) {	/* Y coordinates are within a half-pixel. */
#if MERGE_COLLINEAR_SEGMENTS
	    /* Check for collinear segments. */
	    if (ppt > points + 1 && coord_near(y, ppt[-2].y) &&
		coords_in_order(ppt[-2].x, ptx, x) &&
		coords_in_order(ppt[-2].y, pty, y)
		)
		--ppt;		/* remove middle point */
#endif
	}
#undef coord_near
#undef coords_in_order
	/* Add a line. */
	if (ppt == &points[max_points]) {
	    if (notes & sn_not_first) {
		print_points(points, max_points);
		code = gx_path_add_lines_notes(ppath, points, max_points,
					       notes);
		vd_lineto_multi(points, max_points);
	    } else {
		print_points(points, 1);
		code = gx_path_add_line_notes(ppath, points[0].x,
					      points[0].y, notes);
		vd_lineto(points[0].x, points[0].y);
		if (code < 0)
		    return code;
		print_points(points, max_points - 1);
		code = gx_path_add_lines_notes(ppath, points,
				      max_points - 1, notes | sn_not_first);
		vd_lineto_multi(points, max_points - 1);
	    }
	    if (code < 0)
		return code;
	    ppt = points;
	    notes |= sn_not_first;
	}
	ppt->x = ptx = x;
	ppt->y = pty = y;
	ppt++;
      skip:if (--i == 0)
	    break;		/* don't bother with last accum */
	accum(idx, rdx, id2x, rd2x);
	accum(id2x, rd2x, id3x, rd3x);
	accum(idy, rdy, id2y, rd2y);
	accum(id2y, rd2y, id3y, rd3y);
#undef accum
    }
  last:if_debug4('3', "[3]last x=%g, y=%g x=%d y=%d\n",
	      fixed2float(x3), fixed2float(y3), x3, y3);
    if (ppt > points) {
	int count = ppt + 1 - points;
	gs_fixed_point *pts = points;

	if (!(notes & sn_not_first)) {
	    int code = gx_path_add_line_notes(ppath,
					      points[0].x, points[0].y,
					      notes);

	    print_points(points, 1);
	    vd_lineto(points[0].x, points[0].y);
	    if (code < 0)
		return code;
	    ++pts, --count;
	    notes |= sn_not_first;
	}
	ppt->x = x3, ppt->y = y3;
	print_points(pts, count);
	vd_lineto_multi(pts, count);
	return gx_path_add_lines_notes(ppath, pts, count, notes);
    }
    vd_lineto(x3, y3);
    print_points(&pc->pt, 1);
    return gx_path_add_line_notes(ppath, x3, y3, notes);
}

#undef x1
#undef y1
#undef x2
#undef y2
#undef x3
#undef y3
#undef max_points

#else /* !FLATTENED_CURVE_ITERATOR */

#undef x1
#undef y1
#undef x2
#undef y2
#undef x3
#undef y3

bool
curve_coeffs_ranged(fixed x0, fixed x1, fixed x2, fixed x3, 
		    fixed y0, fixed y1, fixed y2, fixed y3, 
		    fixed *ax, fixed *bx, fixed *cx, 
		    fixed *ay, fixed *by, fixed *cy, 
		    int k)
{
    fixed x01, x12, y01, y12;

    curve_points_to_coefficients(x0, x1, x2, x3, 
				 *ax, *bx, *cx, x01, x12);
    curve_points_to_coefficients(y0, y1, y2, y3, 
				 *ay, *by, *cy, y01, y12);
#   define max_fast (max_fixed / 6)
#   define min_fast (-max_fast)
#   define in_range(v) (v < max_fast && v > min_fast)
    if (k > k_sample_max ||
	!in_range(*ax) || !in_range(*ay) ||
	!in_range(*bx) || !in_range(*by) ||
	!in_range(*cx) || !in_range(*cy)
	)
	return false;
#undef max_fast
#undef min_fast
#undef in_range
    return true;
}

/*  Initialize the iterator. 
    Momotonic curves with non-zero length are only allowed.
 */
bool
gx_flattened_curve_iterator__init(gx_flattened_curve_iterator *this, 
	    fixed x0, fixed y0, const curve_segment *pc, int k, bool reverse, segment_notes notes)
{
    /* Note : Immediately after the ininialization it keeps an invalid (zero length) segment. */
    fixed x1, y1, x2, y2;
    const int k2 = k << 1, k3 = k2 + k;
    fixed bx2, by2, ax6, ay6;

    if (!reverse) {
	x1 = pc->p1.x;
	y1 = pc->p1.y;
	x2 = pc->p2.x;
	y2 = pc->p2.y;
	this->x0 = this->lx0 = this->lx1 = x0;
	this->y0 = this->ly0 = this->ly1 = y0;
	this->x3 = pc->pt.x;
	this->y3 = pc->pt.y;
    } else {
	x1 = pc->p2.x;
	y1 = pc->p2.y;
	x2 = pc->p1.x;
	y2 = pc->p1.y;
	this->x0 = this->lx0 = this->lx1 = pc->pt.x;
	this->y0 = this->ly0 = this->ly1 = pc->pt.y;
	this->x3 = x0;
	this->y3 = y0;
    }
#   if FLATTENED_CURVE_ITERATOR0_COMPATIBLE
#   if CURVED_TRAPEZOID_FILL
    this->reverse = reverse;
#   endif
#   endif
    vd_curve(this->x0, this->y0, x1, y1, x2, y2, this->x3, this->y3, 0, RGB(255, 255, 255));
    this->k = k;
#   ifdef DEBUG
	if (gs_debug_c('3')) {
	    dlprintf4("[3]x0=%f y0=%f x1=%f y1=%f\n",
		      fixed2float(this->x0), fixed2float(this->y0),
		      fixed2float(x1), fixed2float(y1));
	    dlprintf5("   x2=%f y2=%f x3=%f y3=%f  k=%d\n",
		      fixed2float(x2), fixed2float(y2),
		      fixed2float(this->x3), fixed2float(this->y3), this->k);
	}
#   endif
    if (!curve_coeffs_ranged(this->x0, x1, x2, this->x3,
			     this->y0, y1, y2, this->y3, 
			     &this->ax, &this->bx, &this->cx, 
			     &this->ay, &this->by, &this->cy, k))
	return false;
    if (k == -1) {
	/* A special hook for gx_subdivide_curve_rec.
	   Only checked the range. 
	   Returning with no initialization. */
	return true;
    }
    this->rmask = (1 << k3) - 1;
    this->i = (1 << k);
    this->notes = notes;
    this->rx = this->ry = 0;
    if_debug6('3', "[3]ax=%f bx=%f cx=%f\n   ay=%f by=%f cy=%f\n",
	      fixed2float(this->ax), fixed2float(this->bx), fixed2float(this->cx),
	      fixed2float(this->ay), fixed2float(this->by), fixed2float(this->cy));
    bx2 = this->bx << 1;
    by2 = this->by << 1;
    ax6 = ((this->ax << 1) + this->ax) << 1;
    ay6 = ((this->ay << 1) + this->ay) << 1;
    this->idx = arith_rshift(this->cx, this->k);
    this->idy = arith_rshift(this->cy, this->k);
    this->rdx = ((uint)this->cx << k2) & this->rmask;
    this->rdy = ((uint)this->cy << k2) & this->rmask;
    /* bx/y terms */
    this->id2x = arith_rshift(bx2, k2);
    this->id2y = arith_rshift(by2, k2);
    this->rd2x = ((uint)bx2 << this->k) & this->rmask;
    this->rd2y = ((uint)by2 << this->k) & this->rmask;
#   define adjust_rem(r, q, rmask) if ( r > rmask ) q ++, r &= rmask
    /* We can compute all the remainders as ints, */
    /* because we know they don't exceed M. */
    /* cx/y terms */
    this->idx += arith_rshift_1(this->id2x);
    this->idy += arith_rshift_1(this->id2y);
    this->rdx += ((uint)this->bx << this->k) & this->rmask,
    this->rdy += ((uint)this->by << this->k) & this->rmask;
    adjust_rem(this->rdx, this->idx, this->rmask);
    adjust_rem(this->rdy, this->idy, this->rmask);
    /* ax/y terms */
    this->idx += arith_rshift(this->ax, k3);
    this->idy += arith_rshift(this->ay, k3);
    this->rdx += (uint)this->ax & this->rmask;
    this->rdy += (uint)this->ay & this->rmask;
    adjust_rem(this->rdx, this->idx, this->rmask);
    adjust_rem(this->rdy, this->idy, this->rmask);
    this->id2x += this->id3x = arith_rshift(ax6, k3);
    this->id2y += this->id3y = arith_rshift(ay6, k3);
    this->rd2x += this->rd3x = (uint)ax6 & this->rmask,
    this->rd2y += this->rd3y = (uint)ay6 & this->rmask;
    adjust_rem(this->rd2x, this->id2x, this->rmask);
    adjust_rem(this->rd2y, this->id2y, this->rmask);
#   undef adjust_rem
    return true;
}

/*  Initialize the iterator with a line. */
bool
gx_flattened_curve_iterator__init_line(gx_flattened_curve_iterator *this, 
	    fixed x0, fixed y0, const line_segment *pc, segment_notes notes)
{
    this->x0 = this->lx0 = this->lx1 = x0;
    this->y0 = this->ly0 = this->ly1 = y0;
    this->x3 = pc->pt.x;
    this->y3 = pc->pt.y;
    this->k = 1;
    this->i = 1;
    return true;
}

#define coord_near(v, ptv) (!( ((v) ^ (ptv)) & float2fixed(-0.5) ))

/* Move to the next segment and store it to this->lx0, this->ly0, this->lx1, this->ly1 .
 * Return false iff it's a last segment.
 * Note : The number of generated segments can be samller than 2^k 
 *	  due to the empty segment skipping.
 * Note : It can generate collinear segments. 
 */
bool
gx_flattened_curve_iterator__next(gx_flattened_curve_iterator *this)
{
    /*
     * We can compute successive values by finite differences,
     * using the formulas:
     x(t) =
     a*t^3 + b*t^2 + c*t + d =>
     dx(t) = x(t+e)-x(t) =
     a*(3*t^2*e + 3*t*e^2 + e^3) + b*(2*t*e + e^2) + c*e =
     (3*a*e)*t^2 + (3*a*e^2 + 2*b*e)*t + (a*e^3 + b*e^2 + c*e) =>
     d2x(t) = dx(t+e)-dx(t) =
     (3*a*e)*(2*t*e + e^2) + (3*a*e^2 + 2*b*e)*e =
     (6*a*e^2)*t + (6*a*e^3 + 2*b*e^2) =>
     d3x(t) = d2x(t+e)-d2x(t) =
     6*a*e^3;
     x(0) = d, dx(0) = (a*e^3 + b*e^2 + c*e),
     d2x(0) = 6*a*e^3 + 2*b*e^2;
     * In these formulas, e = 1/2^k; of course, there are separate
     * computations for the x and y values.
     *
     * There is a tradeoff in doing the above computation in fixed
     * point.  If we separate out the constant term (d) and require that
     * all the other values fit in a long, then on a 32-bit machine with
     * 12 bits of fraction in a fixed, k = 4 implies a maximum curve
     * size of 128 pixels; anything larger requires subdividing the
     * curve.  On the other hand, doing the computations in explicit
     * double precision slows down the loop by a factor of 3 or so.  We

     * found to our surprise that the latter is actually faster, because
     * the additional subdivisions cost more than the slower loop.
     *
     * We represent each quantity as I+R/M, where I is an "integer" and
     * the "remainder" R lies in the range 0 <= R < M=2^(3*k).  Note
     * that R may temporarily exceed M; for this reason, we require that
     * M have at least one free high-order bit.  To reduce the number of
     * variables, we don't actually compute M, only M-1 (rmask).  */
    fixed x = this->lx1, y = this->ly1;

    this->lx0 = this->lx1;
    this->ly0 = this->ly1;
    /* Fast check for N == 3, a common special case for small characters. */
    if (this->k == 1) {
	if (--this->i == 0)
	    goto last;
#	define poly2(a,b,c) arith_rshift_1(arith_rshift_1(arith_rshift_1(a) + b) + c)
	x += poly2(this->ax, this->bx, this->cx);
	y += poly2(this->ay, this->by, this->cy);
#	undef poly2
	if_debug2('3', "[3]dx=%f, dy=%f\n",
		  fixed2float(x - this->x0), fixed2float(y - this->y0));
	if_debug5('3', "[3]%s x=%g, y=%g x=%d y=%d\n",
		  (((x ^ this->x0) | (y ^ this->y0)) & float2fixed(-0.5) ?
		   "add" : "skip"),
		  fixed2float(x), fixed2float(y), x, y);
	if (
#	if FLATTENED_CURVE_ITERATOR0_COMPATIBLE
#	if CURVED_TRAPEZOID_FILL
	    this->reverse ? ((x ^ this->x3) | (y ^ this->y3)) & float2fixed(-0.5)
		          : ((x ^ this->x0) | (y ^ this->y0)) & float2fixed(-0.5)
#	else
	    ((x ^ this->x0) | (y ^ this->y0)) & float2fixed(-0.5)
#	endif
#	else
	    (((x ^ this->x0) | (y ^ this->y0)) & float2fixed(-0.5)) &&
	    (((x ^ this->x3) | (y ^ this->y3)) & float2fixed(-0.5))
#	endif
	    ) {
	    this->lx1 = x, this->ly1 = y;
	    vd_bar(this->lx0, this->ly0, this->lx1, this->ly1, 1, RGB(0, 255, 0));
	    return true;
	}
    } else {

	for (;;) { /* A loop for skipping short segments -- 
		    those that lie entirely within a square half-pixel. */
	    --this->i;
	    if (!this->i)
		break; /* don't bother with last accum */
#	    ifdef DEBUG
		if (gs_debug_c('3')) {
		    dlprintf4("[3]dx=%f+%d, dy=%f+%d\n",
			      fixed2float(this->idx), this->rdx,
			      fixed2float(this->idy), this->rdy);
		    dlprintf4("   d2x=%f+%d, d2y=%f+%d\n",
			      fixed2float(this->id2x), this->rd2x,
			      fixed2float(this->id2y), this->rd2y);
		    dlprintf4("   d3x=%f+%d, d3y=%f+%d\n",
			      fixed2float(this->id3x), this->rd3x,
			      fixed2float(this->id3y), this->rd3y);
		}
#	    endif
#	    define accum(i, r, di, dr, rmask)\
			if ( (r += dr) > rmask ) r &= rmask, i += di + 1;\
			else i += di
	    accum(x, this->rx, this->idx, this->rdx, this->rmask);
	    accum(y, this->ry, this->idy, this->rdy, this->rmask);
	    accum(this->idx, this->rdx, this->id2x, this->rd2x, this->rmask);
	    accum(this->id2x, this->rd2x, this->id3x, this->rd3x, this->rmask);
	    accum(this->idy, this->rdy, this->id2y, this->rd2y, this->rmask);
	    accum(this->id2y, this->rd2y, this->id3y, this->rd3y, this->rmask);
	    if_debug5('3', "[3]%s x=%g, y=%g x=%d y=%d\n",
		      (((x ^ this->lx0) | (y ^ this->ly0)) & float2fixed(-0.5) ?
		       "add" : "skip"),
		      fixed2float(x), fixed2float(y), x, y);
	    if (!coord_near(x, this->lx0) || !coord_near(y, this->ly0))
		break; /* X coordinates are within a half-pixel. */
	}
#	undef accum
	if (this->i) {
	    this->lx1 = x;
	    this->ly1 = y;
	    vd_bar(this->lx0, this->ly0, this->lx1, this->ly1, 1, RGB(0, 255, 0));
	    return true;
	}
    }
last:
    this->lx1 = this->x3;
    this->ly1 = this->y3;
    if_debug4('3', "[3]last x=%g, y=%g x=%d y=%d\n",
	      fixed2float(this->lx1), fixed2float(this->ly1), this->lx1, this->ly1);
    vd_bar(this->lx0, this->ly0, this->lx1, this->ly1, 1, RGB(0, 255, 0));
    return false;
}

#define max_points 50		/* arbitrary */

private int
generate_segments(gx_path * ppath, const gs_fixed_point *points, 
		    int count, segment_notes notes, bool not_last)
{
    /* vd_moveto(ppath->position.x, ppath->position.y); */
    if (notes & sn_not_first) {
	/* vd_lineto_multi(points, count); */
	print_points(points, count);
	return gx_path_add_lines_notes(ppath, points, count, notes);
    } else {
	int code;

	/* vd_lineto(points[0].x, points[0].y); */
	print_points(points, 1);
	code = gx_path_add_line_notes(ppath, points[0].x, points[0].y, notes);
	if (code < 0)
	    return code;
	/* vd_lineto_multi(points + 1, count - 1); */
#	if FLATTENED_CURVE_ITERATOR0_COMPATIBLE
	if (not_last) {
	    /* Emulating the bug in the old code ! 
	       The first point is duplicated, and the last one is skipped !
	     */
	    print_points(points, count - 1);
	    return gx_path_add_lines_notes(ppath, points, count - 1, notes | sn_not_first);
	} else {
	    print_points(points + 1, count - 1);
	    return gx_path_add_lines_notes(ppath, points + 1, count - 1, notes | sn_not_first);
	}
#	else
	print_points(points + 1, count - 1);
	return gx_path_add_lines_notes(ppath, points + 1, count - 1, notes | sn_not_first);
#endif
    }
}

/*
 * Check for nearly collinear segments -- 
 * those where one coordinate of all three points
 * (the two endpoints and the midpoint) lie within the same
 * half-pixel and both coordinates are monotonic.
 */
private inline bool
gx_check_nearly_collinear_inline(fixed *x0, fixed *y0, fixed *x1, fixed *y1, fixed *x2, fixed *y2)
{
#if MERGE_COLLINEAR_SEGMENTS
    /* fixme: optimise: don't check the coordinate order for monotonic curves. */
#   define coords_in_order(v0, v1, v2) ( (((v1) - (v0)) ^ ((v2) - (v1))) >= 0 )
    if (coord_near(*x2, *x1)) {	/* X coordinates are within a half-pixel. */
	if (coord_near(*x2, *x0) &&
		coords_in_order(*x0, *x1, *x2) &&
		coords_in_order(*y0, *y1, *y2))
	    return true;
    }
    if (coord_near(*y2, *y1)) { /* Y coordinates are within a half-pixel. */
	if (coord_near(*y2, *y0) &&
		coords_in_order(*x0, *x1, *x2) &&
		coords_in_order(*y0, *y1, *y2))
	    return true;
    }
#   undef coords_in_order
#endif
    return false;
}

#if FLATTENED_CURVE_ITERATOR0_COMPATIBLE
bool
gx_check_nearly_collinear(fixed *x0, fixed *y0, fixed *x1, fixed *y1, fixed *x2, fixed *y2)
{
    return gx_check_nearly_collinear_inline(x0, y0, x1, y1, x2, y2);
}
#endif

private int
gx_subdivide_curve_rec(gx_flattened_curve_iterator *this, 
		  gx_path * ppath, int k, curve_segment * pc,
		  segment_notes notes, gs_fixed_point *points)
{
    int code;

top :
    if (!gx_flattened_curve_iterator__init(this, 
		ppath->position.x, ppath->position.y, pc, k, false, notes)) {
	/* Curve is too long.  Break into two pieces and recur. */
	curve_segment cseg;

	k--;
	split_curve_midpoint(ppath->position.x, ppath->position.y, pc, &cseg, pc);
	code = gx_subdivide_curve_rec(this, ppath, k, &cseg, notes, points);
	if (code < 0)
	    return code;
	notes |= sn_not_first;
	goto top;
    } else if (k == -1) {
	/* fixme : Don't need to init the iterator. Just wanted to check in_range. */
	return gx_path_add_curve_notes(ppath, pc->p1.x, pc->p1.y, pc->p2.x, pc->p2.y, 
			pc->pt.x, pc->pt.y, notes);
    } else {
	gs_fixed_point *ppt = points;
	bool not_last;

	for(;;) {
	    not_last = gx_flattened_curve_iterator__next(this);
	    if (ppt > points + 1 && (!FLATTENED_CURVE_ITERATOR0_COMPATIBLE || not_last))
		if (gx_check_nearly_collinear_inline(&ppt[-2].x, &ppt[-2].y, 
				&ppt[-1].x, &ppt[-1].y, &this->lx1, &this->ly1))
		    --ppt;		/* remove middle point */
	    if (!FLATTENED_CURVE_ITERATOR0_COMPATIBLE || !not_last) {
		/* With FLATTENED_CURVE_ITERATOR0_COMPATIBLE it may store points[max_points] 
		   This is only reason for reserving max_points + 1 elements.
		*/
		ppt->x = this->lx1;
		ppt->y = this->ly1;
		ppt++;
	    }
	    if (ppt == &points[max_points] || !not_last) {
#		if FLATTENED_CURVE_ITERATOR0_COMPATIBLE
		gs_fixed_point *pe = ppt;
#		else
		gs_fixed_point *pe = (not_last ?  ppt - 2 : ppt);
#		endif

		code = generate_segments(ppath, points, pe - points, notes, not_last);
		if (code < 0)
		    return code;
		if (!not_last)
		    return 0;
		notes |= sn_not_first;
		memcpy(points, pe, (char *)ppt - (char *)pe);
		ppt = points + (ppt - pe);
	    }
#	    if FLATTENED_CURVE_ITERATOR0_COMPATIBLE
		ppt->x = this->lx1;
		ppt->y = this->ly1;
		ppt++;
#	    endif
	}
    }
}

#undef coord_near

/*
 * Flatten a segment of the path by repeated sampling.
 * 2^k is the number of lines to produce (i.e., the number of points - 1,
 * including the endpoints); we require k >= 1.
 * If k or any of the coefficient values are too large,
 * use recursive subdivision to whittle them down.
 */

int
gx_subdivide_curve(gx_path * ppath, int k, curve_segment * pc, segment_notes notes)
{
    gs_fixed_point points[max_points + 1];
    gx_flattened_curve_iterator iter;

    return gx_subdivide_curve_rec(&iter, ppath, k, pc, notes, points);
}

#undef max_points

#endif /* !FLATTENED_CURVE_ITERATOR */

