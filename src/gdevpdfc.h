/* Copyright (C) 2001 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gdevpdfc.h,v 1.3 2005/12/13 16:57:19 jemarch Exp $ */
/* Internal color space writing interfaces for pdfwrite driver. */

#ifndef gdevpdfc_INCLUDED
#  define gdevpdfc_INCLUDED

/* ------ Exported by gdevpdfc.c for gdevpdfk.c ------ */

/* Define the special cases for CIEBased spaces. */
typedef enum {
    ONE_STEP_NOT,		/* not one-step */
    ONE_STEP_LMN,		/* DecodeLMN (scalar cache) + matrix */
    ONE_STEP_ABC		/* DecodeABC (vector cache) + matrix */
} cie_cache_one_step_t;

/*
 * Finish creating a CIE-based color space (Calxxx or Lab.)
 */
int pdf_finish_cie_space(cos_array_t *pca, cos_dict_t *pcd,
			 const gs_cie_common *pciec);

/* ------ Exported by gdevpdfk.c for gdevpdfc.c ------ */

/*
 * Create an ICCBased color space.  This is a single-use procedure,
 * broken out only for readability.
 */
int pdf_iccbased_color_space(gx_device_pdf *pdev, cos_value_t *pvalue,
			     const gs_color_space *pcs, cos_array_t *pca);

/*
 * Convert a CIEBased space to Lab or ICCBased.
 */
int pdf_convert_cie_space(gx_device_pdf *pdev, cos_array_t *pca,
			  const gs_color_space *pcs, const char *dcsname,
			  const gs_cie_common *pciec, const gs_range *prange,
			  cie_cache_one_step_t one_step,
			  const gs_matrix3 *pmat, const gs_range_t **pprange);

/*
 * Create a Lab color space object.
 */
int pdf_put_lab_color_space(cos_array_t *pca, cos_dict_t *pcd,
			    const gs_range ranges[3] /* only [1] and [2] used */);

#endif /* gdevpdfc_INCLUDED */
