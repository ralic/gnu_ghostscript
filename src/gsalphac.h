/* Copyright (C) 1997 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gsalphac.h,v 1.2 2004/02/14 22:20:16 atai Exp $ */
/* Alpha-compositing interface */

#ifndef gsalphac_INCLUDED
#  define gsalphac_INCLUDED

#include "gscompt.h"

/*
 * Define the compositing operations.  These values must match the ones in
 * dpsNeXT.h.
 */
typedef enum {
    composite_Clear = 0,
    composite_Copy,
    composite_Sover,
    composite_Sin,
    composite_Sout,
    composite_Satop,
    composite_Dover,
    composite_Din,
    composite_Dout,
    composite_Datop,
    composite_Xor,
    composite_PlusD,
    composite_PlusL,
#define composite_last composite_PlusL
    composite_Highlight,	/* (only for compositerect) */
#define compositerect_last composite_Highlight
    composite_Dissolve		/* (not for PostScript composite operators) */
#define composite_op_last composite_Dissolve
} gs_composite_op_t;

/*
 * Define parameters for alpha-compositing.
 */
typedef struct gs_composite_alpha_params_s {
    gs_composite_op_t op;
    float delta;		/* only for Dissolve */
} gs_composite_alpha_params_t;

/* Create an alpha-compositing object. */
int gs_create_composite_alpha(gs_composite_t ** ppcte,
			      const gs_composite_alpha_params_t * params,
			      gs_memory_t * mem);

#endif /* gsalphac_INCLUDED */