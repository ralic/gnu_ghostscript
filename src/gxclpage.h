/* Copyright (C) 1997 artofcode LLC.  All rights reserved.
  
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

/*$Id: gxclpage.h,v 1.1 2004/01/14 16:59:51 atai Exp $ */
/* Command list procedures for saved pages */
/* Requires gdevprn.h, gxclist.h */

#ifndef gxclpage_INCLUDED
#  define gxclpage_INCLUDED

#include "gxclio.h"

/* ---------------- Procedures ---------------- */

/*
 * Package up the current page in a banding device as a page object.
 * The client must provide storage for the page object.
 * The client may retain the object in memory, or may write it on a file
 * for later retrieval; in the latter case, the client should free the
 * in-memory structure.
 */
int gdev_prn_save_page(P3(gx_device_printer * pdev, gx_saved_page * page,
			  int num_copies));

/*
 * Render an array of saved pages by setting up a modified get_bits
 * procedure and then calling the device's normal output_page procedure.
 * Any current page in the device's buffers is lost.
 * The (0,0) point of each saved page is translated to the corresponding
 * specified offset on the combined page.  (Currently the Y offset must be 0.)
 * The client is responsible for freeing the saved and placed pages.
 *
 * Note that the device instance for rendering need not be, and normally is
 * not, the same as the device from which the pages were saved, but it must
 * be an instance of the same device.  The client is responsible for
 * ensuring that the rendering device's buffer size (BufferSpace value) is
 * the same as the BandBufferSpace value of all the saved pages, and that
 * the device width is the same as the BandWidth value of the saved pages.
 */
int gdev_prn_render_pages(P3(gx_device_printer * pdev,
			     const gx_placed_page * ppages, int count));

#endif /* gxclpage_INCLUDED */
