/* Copyright (C) 1997, 1998, 2000 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gstrap.h,v 1.2 2004/02/14 22:20:17 atai Exp $ */
/* Definitions for trapping parameters and zones */

#ifndef gstrap_INCLUDED
#  define gstrap_INCLUDED

#include "gsparam.h"

/* ---------------- Types and structures ---------------- */

/* Opaque type for a path */
#ifndef gx_path_DEFINED
#  define gx_path_DEFINED
typedef struct gx_path_s gx_path;

#endif

/* Define the placement of image traps. */
typedef enum {
    tp_Center,
    tp_Choke,
    tp_Spread,
    tp_Normal
} gs_trap_placement_t;

#define gs_trap_placement_names\
  "Center", "Choke", "Spread", "Normal"

/* Define a trapping parameter set. */
typedef struct gs_trap_params_s {
    float BlackColorLimit;	/* 0-1 */
    float BlackDensityLimit;	/* > 0 */
    float BlackWidth;		/* > 0 */
    /* ColorantZoneDetails; */
    bool Enabled;
    /* HalftoneName; */
    bool ImageInternalTrapping;
    bool ImagemaskTrapping;
    int ImageResolution;
    bool ImageToObjectTrapping;
    gs_trap_placement_t ImageTrapPlacement;
    float SlidingTrapLimit;	/* 0-1 */
    float StepLimit;		/* 0-1 */
    float TrapColorScaling;	/* 0-1 */
    float TrapWidth;		/* > 0 */
} gs_trap_params_t;

/* Define a trapping zone.  ****** SUBJECT TO CHANGE ****** */
typedef struct gs_trap_zone_s {
    gs_trap_params_t params;
    gx_path *zone;
} gs_trap_zone_t;

/* ---------------- Procedures ---------------- */

int gs_settrapparams(gs_trap_params_t * params, gs_param_list * list);

#endif /* gstrap_INCLUDED */
