/* Copyright (C) 1999 artofcode LLC.  All rights reserved.
  
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

/*$Id: gsptype2.c,v 1.1 2004/01/14 16:59:50 atai Exp $ */
/* PatternType 2 implementation */
#include "gx.h"
#include "gscspace.h"
#include "gsshade.h"
#include "gsmatrix.h"           /* for gspcolor.h */
#include "gsstate.h"            /* for set/currentfilladjust */
#include "gxcolor2.h"
#include "gxdcolor.h"
#include "gsptype2.h"
#include "gxpcolor.h"
#include "gxstate.h"            /* for gs_state_memory */
#include "gzpath.h"

/* GC descriptors */
private_st_pattern2_template();
private_st_pattern2_instance();

/* GC procedures */
private ENUM_PTRS_BEGIN(pattern2_instance_enum_ptrs) {
    if (index < st_pattern2_template_max_ptrs) {
        gs_ptr_type_t ptype =
            ENUM_SUPER_ELT(gs_pattern2_instance_t, st_pattern2_template,
                           template, 0);

        if (ptype)
            return ptype;
        return ENUM_OBJ(NULL);  /* don't stop early */
    }
    ENUM_PREFIX(st_pattern_instance, st_pattern2_template_max_ptrs);
}
ENUM_PTRS_END
private RELOC_PTRS_BEGIN(pattern2_instance_reloc_ptrs) {
    RELOC_PREFIX(st_pattern_instance);
    RELOC_SUPER(gs_pattern2_instance_t, st_pattern2_template, template);
} RELOC_PTRS_END

/* Define a PatternType 2 pattern. */
private pattern_proc_uses_base_space(gs_pattern2_uses_base_space);
private pattern_proc_make_pattern(gs_pattern2_make_pattern);
private pattern_proc_get_pattern(gs_pattern2_get_pattern);
private pattern_proc_remap_color(gs_pattern2_remap_color);
private const gs_pattern_type_t gs_pattern2_type = {
    2, {
        gs_pattern2_uses_base_space, gs_pattern2_make_pattern,
        gs_pattern2_get_pattern, gs_pattern2_remap_color
    }
};

/* Initialize a PatternType 2 pattern. */
void
gs_pattern2_init(gs_pattern2_template_t * ppat)
{
    gs_pattern_common_init((gs_pattern_template_t *)ppat, &gs_pattern2_type);
}

/* Test whether a PatternType 2 pattern uses a base space. */
private bool
gs_pattern2_uses_base_space(const gs_pattern_template_t *ptemp)
{
    return false;
}

/* Make an instance of a PatternType 2 pattern. */
private int
gs_pattern2_make_pattern(gs_client_color * pcc,
                         const gs_pattern_template_t * pcp,
                         const gs_matrix * pmat, gs_state * pgs,
                         gs_memory_t * mem)
{
    const gs_pattern2_template_t *ptemp =
        (const gs_pattern2_template_t *)pcp;
    int code = gs_make_pattern_common(pcc, pcp, pmat, pgs, mem,
                                      &st_pattern2_instance);
    gs_pattern2_instance_t *pinst;

    if (code < 0)
        return code;
    pinst = (gs_pattern2_instance_t *)pcc->pattern;
    pinst->template = *ptemp;
    return 0;
}

/* Get the template of a PatternType 2 pattern instance. */
private const gs_pattern_template_t *
gs_pattern2_get_pattern(const gs_pattern_instance_t *pinst)
{
    return (const gs_pattern_template_t *)
        &((const gs_pattern2_instance_t *)pinst)->template;
}

/* ---------------- Rendering ---------------- */

/* GC descriptor */
gs_private_st_ptrs_add0(st_dc_pattern2, gx_device_color, "dc_pattern2",
                        dc_pattern2_enum_ptrs, dc_pattern2_reloc_ptrs,
                        st_client_color, ccolor);

private dev_color_proc_load(gx_dc_pattern2_load);
private dev_color_proc_fill_rectangle(gx_dc_pattern2_fill_rectangle);
private dev_color_proc_equal(gx_dc_pattern2_equal);
/*
 * Define the PatternType 2 Pattern device color type.  This is public only
 * for testing when writing PDF or PostScript.
 */
const gx_device_color_type_t gx_dc_pattern2 = {
    &st_dc_pattern2,
    gx_dc_pattern2_load, gx_dc_pattern2_fill_rectangle,
    gx_dc_default_fill_masked, gx_dc_pattern2_equal
};

/* Check device color for Pattern Type 2. */
bool
gx_dc_is_pattern2_color(const gx_device_color *pdevc)
{
    return pdevc->type == &gx_dc_pattern2;
}

/* Load a PatternType 2 color into the cache.  (No effect.) */
private int
gx_dc_pattern2_load(gx_device_color *pdevc, const gs_imager_state *ignore_pis,
                    gx_device *ignore_dev, gs_color_select_t ignore_select)
{
    return 0;
}

/* Remap a PatternType 2 color. */
private int
gs_pattern2_remap_color(const gs_client_color * pc, const gs_color_space * pcs,
                        gx_device_color * pdc, const gs_imager_state * pis,
                        gx_device * dev, gs_color_select_t select)
{
    /* We don't do any actual color mapping now. */
    pdc->type = &gx_dc_pattern2;
    pdc->ccolor = *pc;
    return 0;
}

/* Fill path or rect, with adjustment, and with a PatternType 2 color. */
int
gx_dc_pattern2_fill_path_adjusted(const gx_device_color * pdevc, 
                              gx_path * ppath, gs_fixed_rect * rect, 
                              gx_device * dev)
{
    gs_pattern2_instance_t *pinst =
        (gs_pattern2_instance_t *)pdevc->ccolor.pattern;
    gs_state *pgs = pinst->saved;
    gs_point save_adjust;
    int code;

    /* We don't want any adjustment of the box. */
    gs_currentfilladjust(pgs, &save_adjust);

    /*
     * We should set the fill adjustment to zero here, so that we don't
     * get multiply-written pixels as a result of filling abutting
     * triangles.  However, numerical inaccuracies in the shading
     * algorithms can cause pixel dropouts, and a non-zero adjustment
     * is by far the easiest way to work around them as a stopgap.
     * NOTE: This makes shadings not interact properly with
     * non-idempotent RasterOps (not a problem in practice, since
     * PostScript doesn't have RasterOps and PCL doesn't have shadings).
     */
    gs_setfilladjust(pgs, 0.5, 0.5);
    /****** DOESN'T HANDLE RASTER OP ******/
    code = gs_shading_fill_path(pinst->template.Shading, ppath, rect, dev,
                                (gs_imager_state *)pgs, true);
    gs_setfilladjust(pgs, save_adjust.x, save_adjust.y);
    return code;
}

/* Fill a rectangle with a PatternType 2 color. */
private int
gx_dc_pattern2_fill_rectangle(const gx_device_color * pdevc, int x, int y,
                              int w, int h, gx_device * dev,
                              gs_logical_operation_t lop,
                              const gx_rop_source_t * source)
{
    gs_fixed_rect rect;
    rect.p.x = int2fixed(x);
    rect.p.y = int2fixed(y);
    rect.q.x = int2fixed(x + w);
    rect.q.y = int2fixed(y + h);
    return gx_dc_pattern2_fill_path_adjusted(pdevc, NULL, &rect,  dev);
}

/* Compare two PatternType 2 colors for equality. */
private bool
gx_dc_pattern2_equal(const gx_device_color * pdevc1,
                     const gx_device_color * pdevc2)
{
    return pdevc2->type == pdevc1->type &&
        pdevc1->ccolor.pattern == pdevc2->ccolor.pattern;
}
