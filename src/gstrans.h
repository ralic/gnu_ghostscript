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

/* $Id: gstrans.h,v 1.2 2004/02/14 22:20:17 atai Exp $ */
/* Transparency definitions and interface */

#ifndef gstrans_INCLUDED
#  define gstrans_INCLUDED

#include "gstparam.h"

/* Access transparency-related graphics state elements. */
int gs_setblendmode(gs_state *, gs_blend_mode_t);
gs_blend_mode_t gs_currentblendmode(const gs_state *);
int gs_setopacityalpha(gs_state *, floatp);
float gs_currentopacityalpha(const gs_state *);
int gs_setshapealpha(gs_state *, floatp);
float gs_currentshapealpha(const gs_state *);
int gs_settextknockout(gs_state *, bool);
bool gs_currenttextknockout(const gs_state *);

/*
 * Manage transparency group and mask rendering.  Eventually these will be
 * driver procedures, taking dev + pis instead of pgs.
 */

gs_transparency_state_type_t
    gs_current_transparency_type(const gs_state *pgs);

/*
 * We have to abbreviate the procedure name because procedure names are
 * only unique to 23 characters on VMS.
 */
void gs_trans_group_params_init(gs_transparency_group_params_t *ptgp);

int gs_begin_transparency_group(gs_state *pgs,
				const gs_transparency_group_params_t *ptgp,
				const gs_rect *pbbox);

int gs_end_transparency_group(gs_state *pgs);

void gs_trans_mask_params_init(gs_transparency_mask_params_t *ptmp,
			       gs_transparency_mask_subtype_t subtype);

int gs_begin_transparency_mask(gs_state *pgs,
			       const gs_transparency_mask_params_t *ptmp,
			       const gs_rect *pbbox);

int gs_end_transparency_mask(gs_state *pgs,
			     gs_transparency_channel_selector_t csel);

int gs_init_transparency_mask(gs_state *pgs,
			      gs_transparency_channel_selector_t csel);

int gs_discard_transparency_layer(gs_state *pgs);

#endif /* gstrans_INCLUDED */
