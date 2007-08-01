/* Copyright (C) 2001-2006 artofcode LLC.
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

/* $Id: gstparam.h,v 1.7 2007/08/01 14:26:14 jemarch Exp $ */
/* Transparency parameter definitions */

#ifndef gstparam_INCLUDED
#  define gstparam_INCLUDED

#include "gsccolor.h"
#include "gsrefct.h"

/* Define the names of the known blend modes. */
typedef enum {
    BLEND_MODE_Compatible,
    BLEND_MODE_Normal,
    BLEND_MODE_Multiply,
    BLEND_MODE_Screen,
    BLEND_MODE_Difference,
    BLEND_MODE_Darken,
    BLEND_MODE_Lighten,
    BLEND_MODE_ColorDodge,
    BLEND_MODE_ColorBurn,
    BLEND_MODE_Exclusion,
    BLEND_MODE_HardLight,
    BLEND_MODE_Overlay,
    BLEND_MODE_SoftLight,
    BLEND_MODE_Luminosity,
    BLEND_MODE_Hue,
    BLEND_MODE_Saturation,
    BLEND_MODE_Color
#define MAX_BLEND_MODE BLEND_MODE_Color
} gs_blend_mode_t;
#define GS_BLEND_MODE_NAMES\
  "Compatible", "Normal", "Multiply", "Screen", "Difference",\
  "Darken", "Lighten", "ColorDodge", "ColorBurn", "Exclusion",\
  "HardLight", "Overlay", "SoftLight", "Luminosity", "Hue",\
  "Saturation", "Color"

/* Define the common part for a transparency stack state. */
typedef enum {
    TRANSPARENCY_STATE_Group = 1,	/* must not be 0 */
    TRANSPARENCY_STATE_Mask
} gs_transparency_state_type_t;
#define GS_TRANSPARENCY_STATE_COMMON\
    gs_transparency_state_t *saved;\
    gs_transparency_state_type_t type
typedef struct gs_transparency_state_s gs_transparency_state_t;
struct gs_transparency_state_s {
    GS_TRANSPARENCY_STATE_COMMON;
};

/* Define the common part for a cached transparency mask. */
#define GS_TRANSPARENCY_MASK_COMMON\
    rc_header rc
typedef struct gs_transparency_mask_s {
    GS_TRANSPARENCY_MASK_COMMON;
} gs_transparency_mask_t;

/* Define the parameter structure for a transparency group. */
#ifndef gs_color_space_DEFINED
#  define gs_color_space_DEFINED
typedef struct gs_color_space_s gs_color_space;
#endif
#ifndef gs_function_DEFINED
typedef struct gs_function_s gs_function_t;
#  define gs_function_DEFINED
#endif

/* (Update gs_trans_group_params_init if these change.) */
typedef struct gs_transparency_group_params_s {
    const gs_color_space *ColorSpace;
    bool Isolated;
    bool Knockout;
    bool image_with_SMask;
} gs_transparency_group_params_t;

/* Define the parameter structure for a transparency mask. */
typedef enum {
    TRANSPARENCY_MASK_Alpha,
    TRANSPARENCY_MASK_Luminosity
} gs_transparency_mask_subtype_t;

#define GS_TRANSPARENCY_MASK_SUBTYPE_NAMES\
  "Alpha", "Luminosity"

/* See the gx_transparency_mask_params_t type below */
/* (Update gs_trans_mask_params_init if these change.) */
typedef struct gs_transparency_mask_params_s {
    gs_transparency_mask_subtype_t subtype;
    int Background_components;
    float Background[GS_CLIENT_COLOR_MAX_COMPONENTS];
    float GrayBackground;
    int (*TransferFunction)(floatp in, float *out, void *proc_data);
    gs_function_t *TransferFunction_data;
} gs_transparency_mask_params_t;

#define MASK_TRANSFER_FUNCTION_SIZE 256

/* The post clist version of transparency mask parameters */
typedef struct gx_transparency_mask_params_s {
    gs_transparency_mask_subtype_t subtype;
    int Background_components;
    float Background[GS_CLIENT_COLOR_MAX_COMPONENTS];
    float GrayBackground;
    bool function_is_identity;
    byte transfer_fn[MASK_TRANSFER_FUNCTION_SIZE];
} gx_transparency_mask_params_t;

/* Select the opacity or shape parameters. */
typedef enum {
    TRANSPARENCY_CHANNEL_Opacity = 0,
    TRANSPARENCY_CHANNEL_Shape = 1
} gs_transparency_channel_selector_t;

#endif /* gstparam_INCLUDED */
