/* Copyright (C) 1997, 2000 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gdevpdfd.c,v 1.2 2004/02/14 22:20:05 atai Exp $ */
/* Path drawing procedures for pdfwrite driver */
#include "math_.h"
#include "gx.h"
#include "gxdevice.h"
#include "gxfixed.h"
#include "gxistate.h"
#include "gxpaint.h"
#include "gzpath.h"
#include "gzcpath.h"
#include "gdevpdfx.h"
#include "gdevpdfg.h"

/* ---------------- Drawing ---------------- */

/* Fill a rectangle. */
int
gdev_pdf_fill_rectangle(gx_device * dev, int x, int y, int w, int h,
			gx_color_index color)
{
    gx_device_pdf *pdev = (gx_device_pdf *) dev;
    int code;

    /* Make a special check for the initial fill with white, */
    /* which shouldn't cause the page to be opened. */
    if (color == pdev->white && !is_in_page(pdev))
	return 0;
    code = pdf_open_page(pdev, PDF_IN_STREAM);
    if (code < 0)
	return code;
    /* Make sure we aren't being clipped. */
    pdf_put_clip_path(pdev, NULL);
    pdf_set_pure_color(pdev, color, &pdev->fill_color,
		       &psdf_set_fill_color_commands);
    pprintd4(pdev->strm, "%d %d %d %d re f\n", x, y, w, h);
    return 0;
}

/* ---------------- Path drawing ---------------- */

/* ------ Vector device implementation ------ */

private int
pdf_setlinewidth(gx_device_vector * vdev, floatp width)
{
    /* Acrobat Reader doesn't accept negative line widths. */
    return psdf_setlinewidth(vdev, fabs(width));
}

private int
pdf_setfillcolor(gx_device_vector * vdev, const gx_drawing_color * pdc)
{
    gx_device_pdf *const pdev = (gx_device_pdf *)vdev;

    return pdf_set_drawing_color(pdev, pdc, &pdev->fill_color,
				 &psdf_set_fill_color_commands);
}

private int
pdf_setstrokecolor(gx_device_vector * vdev, const gx_drawing_color * pdc)
{
    gx_device_pdf *const pdev = (gx_device_pdf *)vdev;

    return pdf_set_drawing_color(pdev, pdc, &pdev->stroke_color,
				 &psdf_set_stroke_color_commands);
}

private int
pdf_dorect(gx_device_vector * vdev, fixed x0, fixed y0, fixed x1, fixed y1,
	   gx_path_type_t type)
{
    fixed xmax = int2fixed(vdev->width), ymax = int2fixed(vdev->height);
    fixed xmin = 0, ymin = 0;

    /*
     * If we're doing a stroke operation, expand the checking box by the
     * stroke width.
     */
    if (type & gx_path_type_stroke) {
	double w = vdev->state.line_params.half_width;
	double xw = w * (fabs(vdev->state.ctm.xx) + fabs(vdev->state.ctm.yx));
	double yw = w * (fabs(vdev->state.ctm.xy) + fabs(vdev->state.ctm.yy));

	xmin = -(float2fixed(xw) + fixed_1);
	xmax -= xmin;
	ymin = -(float2fixed(yw) + fixed_1);
	ymax -= ymin;
    }
    if (!(type & gx_path_type_clip) &&
	(x0 > xmax || x1 < xmin || y0 > ymax || y1 < ymin ||
	 x0 > x1 || y0 > y1)
	)
	return 0;		/* nothing to fill or stroke */
    /*
     * Clamp coordinates to avoid tripping over Acrobat Reader's limit
     * of 32K on user coordinate values.
     */
    if (x0 < xmin)
	x0 = xmin;
    if (x1 > xmax)
	x1 = xmax;
    if (y0 < ymin)
	y0 = ymin;
    if (y1 > ymax)
	y1 = ymax;
    return psdf_dorect(vdev, x0, y0, x1, y1, type);
}

private int
pdf_endpath(gx_device_vector * vdev, gx_path_type_t type)
{
    return 0;			/* always handled by caller */
}

const gx_device_vector_procs pdf_vector_procs = {
	/* Page management */
    NULL,
	/* Imager state */
    pdf_setlinewidth,
    psdf_setlinecap,
    psdf_setlinejoin,
    psdf_setmiterlimit,
    psdf_setdash,
    psdf_setflat,
    psdf_setlogop,
	/* Other state */
    pdf_setfillcolor,
    pdf_setstrokecolor,
	/* Paths */
    psdf_dopath,
    pdf_dorect,
    psdf_beginpath,
    psdf_moveto,
    psdf_lineto,
    psdf_curveto,
    psdf_closepath,
    pdf_endpath
};

/* ------ Utilities ------ */

/* Test whether we will need to put the clipping path. */
bool
pdf_must_put_clip_path(gx_device_pdf * pdev, const gx_clip_path * pcpath)
{
    if (pcpath == NULL)
	return pdev->clip_path_id != pdev->no_clip_path_id;
    if (pdev->clip_path_id == pcpath->id)
	return false;
    if (gx_cpath_includes_rectangle(pcpath, fixed_0, fixed_0,
				    int2fixed(pdev->width),
				    int2fixed(pdev->height))
	)
	return pdev->clip_path_id != pdev->no_clip_path_id;
    return true;
}

/* Put a clipping path on the output file. */
int
pdf_put_clip_path(gx_device_pdf * pdev, const gx_clip_path * pcpath)
{
    int code;
    stream *s = pdev->strm;
    gs_id new_id;

    /* Check for no update needed. */
    if (pcpath == NULL) {
	if (pdev->clip_path_id == pdev->no_clip_path_id)
	    return 0;
	new_id = pdev->no_clip_path_id;
    } else {
	if (pdev->clip_path_id == pcpath->id)
	    return 0;
	new_id = pcpath->id;
	if (gx_cpath_includes_rectangle(pcpath, fixed_0, fixed_0,
					int2fixed(pdev->width),
					int2fixed(pdev->height))
	    ) {
	    if (pdev->clip_path_id == pdev->no_clip_path_id)
		return 0;
	    new_id = pdev->no_clip_path_id;
	}
    }
    /*
     * The contents must be open already, so the following will only exit
     * text or string context.
     */
    code = pdf_open_contents(pdev, PDF_IN_STREAM);
    if (code < 0)
	return 0;
    stream_puts(s, "Q\nq\n");
    if (new_id != pdev->no_clip_path_id) {
	gdev_vector_dopath_state_t state;
	gs_cpath_enum cenum;
	gs_fixed_point vs[3];
	int pe_op;

	gdev_vector_dopath_init(&state, (gx_device_vector *)pdev,
				gx_path_type_fill, NULL);
	/*
	 * We have to break 'const' here because the clip path
	 * enumeration logic uses some internal mark bits.
	 * This is very unfortunate, but until we can come up with
	 * a better algorithm, it's necessary.
	 */
	gx_cpath_enum_init(&cenum, (gx_clip_path *) pcpath);
	while ((pe_op = gx_cpath_enum_next(&cenum, vs)) > 0)
	    gdev_vector_dopath_segment(&state, pe_op, vs);
	pprints1(s, "%s n\n", (pcpath->rule <= 0 ? "W" : "W*"));
	if (pe_op < 0)
	    return pe_op;
    }
    pdev->clip_path_id = new_id;
    pdf_reset_graphics(pdev);
    return 0;
}

/*
 * Compute the scaling to ensure that user coordinates for a path are within
 * Acrobat's range.  Return true if scaling was needed.  In this case, the
 * CTM will be multiplied by *pscale, and all coordinates will be divided by
 * *pscale.
 */
private bool
make_path_scaling(const gx_device_pdf *pdev, gx_path *ppath,
		  floatp prescale, double *pscale)
{
    gs_fixed_rect bbox;
    double bmin, bmax;

    gx_path_bbox(ppath, &bbox);
    bmin = min(bbox.p.x / pdev->scale.x, bbox.p.y / pdev->scale.y) * prescale;
    bmax = max(bbox.q.x / pdev->scale.x, bbox.q.y / pdev->scale.y) * prescale;
    if (bmin <= int2fixed(-MAX_USER_COORD) ||
	bmax > int2fixed(MAX_USER_COORD)
	) {
	/* Rescale the path. */
	*pscale = max(bmin / int2fixed(-MAX_USER_COORD),
		      bmax / int2fixed(MAX_USER_COORD));
	return true;
    } else {
	*pscale = 1;
	return false;
    }
#undef MAX_USER_COORD
}

/* ------ Driver procedures ------ */

/* Fill a path. */
int
gdev_pdf_fill_path(gx_device * dev, const gs_imager_state * pis, gx_path * ppath,
		   const gx_fill_params * params,
	      const gx_drawing_color * pdcolor, const gx_clip_path * pcpath)
{
    gx_device_pdf *pdev = (gx_device_pdf *) dev;
    int code;
    /*
     * HACK: we fill an empty path in order to set the clipping path
     * and the color for writing text.  If it weren't for this, we
     * could detect and skip empty paths before putting out the clip
     * path or the color.  We also clip with an empty path in order
     * to advance currentpoint for show operations without actually
     * drawing anything.
     */
    bool have_path;
    gs_fixed_rect box = {{0, 0}, {0, 0}};

    /*
     * Check for an empty clipping path.
     */
    if (pcpath) {
	gx_cpath_outer_box(pcpath, &box);
	if (box.p.x >= box.q.x || box.p.y >= box.q.y)
	    return 0;		/* empty clipping path */
    }
    code = pdf_prepare_fill(pdev, pis);
    if (code < 0)
	return code;
    if (gx_dc_is_pure(pdcolor)) {
	/*
	 * Make a special check for the initial fill with white,
	 * which shouldn't cause the page to be opened.
	 */
	if (gx_dc_pure_color(pdcolor) == pdev->white && !is_in_page(pdev))
	    return 0;
    }
    have_path = !gx_path_is_void(ppath);
    if (have_path || pdev->context == PDF_IN_NONE ||
	pdf_must_put_clip_path(pdev, pcpath)
	) {
	code = pdf_open_page(pdev, PDF_IN_STREAM);
	if (code < 0)
	    return code;
    }
    pdf_put_clip_path(pdev, pcpath);
    if (pdf_setfillcolor((gx_device_vector *)pdev, pdcolor) < 0)
	return gx_default_fill_path(dev, pis, ppath, params, pdcolor,
				    pcpath);
    if (have_path) {
	stream *s = pdev->strm;
	double scale;
	gs_matrix smat;
	gs_matrix *psmat = NULL;
	
	if (pcpath) {
	    gs_fixed_rect box1;

	    code = gx_path_bbox(ppath, &box1);
	    if (code < 0)
		return code;
 	    rect_intersect(box1, box);
 	    if (box1.p.x > box1.q.x || box1.p.y > box1.q.y)
  		return 0;		/* outside the clipping path */
	}
	if (params->flatness != pdev->state.flatness) {
	    pprintg1(s, "%g i\n", params->flatness);
	    pdev->state.flatness = params->flatness;
	}
	if (make_path_scaling(pdev, ppath, 1.0, &scale)) {
	    gs_make_scaling(pdev->scale.x * scale, pdev->scale.y * scale,
			    &smat);
            pdf_put_matrix(pdev, "q ", &smat, "cm\n");
	    psmat = &smat;
	}
	gdev_vector_dopath((gx_device_vector *)pdev, ppath,
			   gx_path_type_fill | gx_path_type_optimize,
			   psmat);
	stream_puts(s, (params->rule < 0 ? "f\n" : "f*\n"));
	if (psmat)
	    stream_puts(s, "Q\n");
    }
    return 0;
}

/* Stroke a path. */
int
gdev_pdf_stroke_path(gx_device * dev, const gs_imager_state * pis,
		     gx_path * ppath, const gx_stroke_params * params,
	      const gx_drawing_color * pdcolor, const gx_clip_path * pcpath)
{
    gx_device_pdf *pdev = (gx_device_pdf *) dev;
    stream *s;
    int code;
    double scale, path_scale;
    bool set_ctm;
    gs_matrix mat;
    double prescale = 1;

    if (gx_path_is_void(ppath))
	return 0;		/* won't mark the page */
    code = pdf_prepare_stroke(pdev, pis);
    if (code < 0)
	return code;
    code = pdf_open_page(pdev, PDF_IN_STREAM);
    if (code < 0)
	return code;
    /*
     * If the CTM is not uniform, stroke width depends on angle.
     * We'd like to avoid resetting the CTM, so we check for uniform
     * CTMs explicitly.  Note that in PDF, unlike PostScript, it is
     * the CTM at the time of the stroke operation, not the CTM at
     * the time the path was constructed, that is used for transforming
     * the points of the path; so if we have to reset the CTM, we must
     * do it before constructing the path, and inverse-transform all
     * the coordinates.
     */
    set_ctm = (bool)gdev_vector_stroke_scaling((gx_device_vector *)pdev,
					       pis, &scale, &mat);
    if (set_ctm) {
	/*
	 * We want a scaling factor that will bring the largest reasonable
	 * user coordinate within bounds.  We choose a factor based on the
	 * minor axis of the transformation.  Thanks to Raph Levien for
	 * the following formula.
	 */
	double a = mat.xx, b = mat.xy, c = mat.yx, d = mat.yy;
	double u = fabs(a * d - b * c);
	double v = a * a + b * b + c * c + d * d;
	double minor = (sqrt(v + 2 * u) - sqrt(v - 2 * u)) * 0.5;

	prescale = (minor == 0 || minor > 1 ? 1 : 1 / minor);
    }
    if (make_path_scaling(pdev, ppath, prescale, &path_scale)) {
	scale /= path_scale;
	if (set_ctm)
	    gs_matrix_scale(&mat, path_scale, path_scale, &mat);
	else {
	    gs_make_scaling(path_scale, path_scale, &mat);
	    set_ctm = true;
	}
    }
    pdf_put_clip_path(pdev, pcpath);
    code = gdev_vector_prepare_stroke((gx_device_vector *)pdev, pis, params,
				      pdcolor, scale);
    if (code < 0)
	return gx_default_stroke_path(dev, pis, ppath, params, pdcolor,
				      pcpath);

    if (set_ctm)
  	pdf_put_matrix(pdev, "q ", &mat, "cm\n");
    code = gdev_vector_dopath((gx_device_vector *)pdev, ppath,
			      gx_path_type_stroke | gx_path_type_optimize,
			      (set_ctm ? &mat : (const gs_matrix *)0));
    if (code < 0)
	return code;
    s = pdev->strm;
    stream_puts(s, (code ? "s" : "S"));
    stream_puts(s, (set_ctm ? " Q\n" : "\n"));
    return 0;
}