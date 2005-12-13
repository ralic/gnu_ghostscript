/* Copyright (C) 2002 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gdevpdts.h,v 1.4 2005/12/13 16:57:19 jemarch Exp $ */
/* Text state structure and API for pdfwrite */

#ifndef gdevpdts_INCLUDED
#  define gdevpdts_INCLUDED

#include "gsmatrix.h"

/*
 * See gdevpdtt.h for a discussion of the multiple coordinate systems that
 * the text code must use.
 */

/* ================ Types and structures ================ */

#ifndef pdf_text_state_DEFINED
#  define pdf_text_state_DEFINED
typedef struct pdf_text_state_s pdf_text_state_t;
#endif

/*
 * Clients pass in the text state values; the implementation decides when
 * (and, in the case of text positioning, how) to emit PDF commands to
 * set them in the output.
 */
typedef struct pdf_text_state_values_s {
    float character_spacing;	/* Tc */
    pdf_font_resource_t *pdfont; /* for Tf */
    double size;		/* for Tf */
    /*
     * The matrix is the transformation from text space to user space, which
     * in pdfwrite text output is the same as device space.  Thus this
     * matrix combines the effect of the PostScript CTM and the FontMatrix,
     * scaled by the inverse of the font size value.
     */
    gs_matrix matrix;		/* Tm et al */
    int render_mode;		/* Tr */
    float word_spacing;		/* Tw */
} pdf_text_state_values_t;
#define TEXT_STATE_VALUES_DEFAULT\
    0,				/* character_spacing */\
    NULL,			/* font */\
    0,				/* size */\
    { identity_matrix_body },	/* matrix */\
    0,				/* render_mode */\
    0				/* word_spacing */

/* ================ Procedures ================ */

/* ------ Exported for gdevpdfu.c ------ */

/*
 * Transition from stream context to text context.
 */
int pdf_from_stream_to_text(gx_device_pdf *pdev);

/*
 * Transition from string context to text context.
 */
int pdf_from_string_to_text(gx_device_pdf *pdev);

/*
 * Close the text aspect of the current contents part.
 */
void pdf_close_text_contents(gx_device_pdf *pdev); /* gdevpdts.h */

/* ------ Used only within the text code ------ */

/*
 * Test whether a change in render_mode requires resetting the stroke
 * parameters.
 */
bool pdf_render_mode_uses_stroke(const gx_device_pdf *pdev,
				 const pdf_text_state_values_t *ptsv);

/*
 * Read the stored client view of text state values.
 */
void pdf_get_text_state_values(gx_device_pdf *pdev,
			       pdf_text_state_values_t *ptsv);

/*
 * Set wmode to text state.
 */
void pdf_set_text_wmode(gx_device_pdf *pdev, int wmode);


/*
 * Set the stored client view of text state values.
 */
int pdf_set_text_state_values(gx_device_pdf *pdev,
			      const pdf_text_state_values_t *ptsv);

/*
 * Transform a distance from unscaled text space (text space ignoring the
 * scaling implied by the font size) to device space.
 */
int pdf_text_distance_transform(floatp wx, floatp wy,
				const pdf_text_state_t *pts,
				gs_point *ppt);

/*
 * Return the current (x,y) text position as seen by the client, in
 * unscaled text space.
 */
void pdf_text_position(const gx_device_pdf *pdev, gs_point *ppt);

/*
 * Append characters to text being accumulated, giving their advance width
 * in device space.
 */
int pdf_append_chars(gx_device_pdf * pdev, const byte * str, uint size,
		     floatp wx, floatp wy, bool nobreak);

#endif /* gdevpdts_INCLUDED */
