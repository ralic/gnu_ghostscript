/* Copyright (C) 2000 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gsipar3x.h,v 1.2 2004/02/14 22:20:17 atai Exp $ */
/* Extended ImageType 3 ("3x") image parameter definition */

#ifndef gsipar3x_INCLUDED
#  define gsipar3x_INCLUDED

#include "gsiparam.h"
#include "gsiparm3.h"		/* for interleave types */

/*
 * An ImageType 3x image is the transparency-capable extension of an
 * ImageType 3 image.  Instead of a MaskDict, it has an OpacityMaskDict
 * and/or a ShapeMaskDict, whose depths may be greater than 1.  It also
 * has an optional Matte member, defining a pre-mixed background color.
 *
 * Since ImageTypes must be numeric, we assign the number 103 for this
 * type of image; however, this is defined in only one place, namely here.
 */
#define IMAGE3X_IMAGETYPE 103

/*
 * If InterleaveType is 3, the data source(s) for the mask(s) *precede* the
 * data sources for the pixel data, with opacity preceding shape.  For
 * InterleaveType 3, the client is responsible for always providing mask
 * data before the pixel data that it masks.  (The implementation does not
 * currently check this, but it should.)  For this type of image,
 * InterleaveType 2 (interleaved scan lines) is not allowed.
 */
typedef struct gs_image3x_mask_s {
    int InterleaveType;
    float Matte[GS_CLIENT_COLOR_MAX_COMPONENTS];
    bool has_Matte;
    /*
     * Note that the ColorSpaces in the MaskDicts are ignored.
     * Note also that MaskDict.BitsPerComponent may be zero, which
     * indicates that the given mask is not supplied.
     */
    gs_data_image_t MaskDict;
} gs_image3x_mask_t;
typedef struct gs_image3x_s {
    gs_pixel_image_common;	/* DataDict */
    gs_image3x_mask_t Opacity, Shape; /* ...MaskDict */
} gs_image3x_t;

/* As noted above, the ColorSpaces in the MaskDicts are ignored. */
#define private_st_gs_image3x()	/* in gximag3x.c */\
  gs_private_st_suffix_add0(st_gs_image3x, gs_image3x_t, "gs_image3x_t",\
    image3x_enum_ptrs, image3x_reloc_ptrs, st_gs_pixel_image)

/*
 * Initialize an ImageType 3x image.
 */
void gs_image3x_t_init(gs_image3x_t *pim, const gs_color_space *color_space);

#endif /* gsipar3x_INCLUDED */