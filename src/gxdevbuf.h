/* Copyright (C) 2001-2006 Artifex Software, Inc.
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

/* $Id: gxdevbuf.h,v 1.8 2007/09/11 15:24:32 Arabidopsis Exp $ */
/* Definitions for device buffer management */

#ifndef gxdevbuf_INCLUDED
#  define gxdevbuf_INCLUDED

#include "gxrplane.h"		/* for _buf_device procedures */

/*
 * Define the procedures for managing rendering buffers.  These are
 * currently associated with printer and/or banded devices, but they
 * might have broader applicability in the future.
 *
 * For "async" devices, size_buf_device may be called by either the
 * writer or the reader thread; the other procedures may be called only
 * by the reader thread.
 */

#ifndef gx_device_DEFINED
#  define gx_device_DEFINED
typedef struct gx_device_s gx_device;
#endif

/* Define the structure for returning buffer space requirements. */
typedef struct gx_device_buf_space_s {
    ulong bits;
    ulong line_ptrs;
    uint raster;
} gx_device_buf_space_t;

/* clist contents analysis per band */
typedef struct gx_band_complexity_s {
    /* use null or contents to denote non banding case: */

    bool uses_color;
    bool nontrivial_rops;

#if 0
    /* halftone phase */
    int x0;
    int y0;
#endif
} gx_band_complexity_t;


typedef struct gx_device_buf_procs_s {

    /*
     * Create the buffer device(s) for the pixels or a plane of a page
     * or band.  We use 'buf' instead of buffer because VMS limits
     * procedure names to 31 characters.  Note that the client must
     * fully initialize the render_plane using gx_render_plane_init.
     *
     * If mem is NULL, *pbdev must already point to a gx_device_memory,
     * and this procedure must initialize it.  If this isn't possible
     * (e.g., if render_plane calls for a single plane),
     * create_buf_device must return an error.
     */

#define dev_proc_create_buf_device(proc)\
  int proc(gx_device **pbdev, gx_device *target,\
	   const gx_render_plane_t *render_plane, gs_memory_t *mem,\
	   gx_band_complexity_t *band_complexity)

    dev_proc_create_buf_device((*create_buf_device));

    /*
     * Return the amount of buffer space needed by setup_buf_device.
     */

#define dev_proc_size_buf_device(proc)\
  int proc(gx_device_buf_space_t *space, gx_device *target,\
	   const gx_render_plane_t *render_plane,\
	   int height, bool for_band)

    dev_proc_size_buf_device((*size_buf_device));

    /*
     * Set up the buffer device with a specific buffer.
     * If line_ptrs is not NULL, it points to an allocated area for
     * the scan line pointers of an eventual memory device.
     *
     * Note that this procedure is used for two different purposes:
     * setting up a full band buffer for rendering, and setting up a
     * partial-band buffer device for reading out selected scan lines.
     * The latter case requires that we also pass the full height of the
     * buffer, for multi-planar memory devices; in the former case,
     * y = 0 and setup_height = full_height.
     */

#define dev_proc_setup_buf_device(proc)\
  int proc(gx_device *bdev, byte *buffer, int bytes_per_line,\
	   byte **line_ptrs /*[height]*/, int y, int setup_height,\
	   int full_height)

    dev_proc_setup_buf_device((*setup_buf_device));

    /*
     * Destroy the buffer device and all associated structures.
     * Note that this does *not* destroy the buffered data.
     */

#define dev_proc_destroy_buf_device(proc)\
  void proc(gx_device *bdev)

    dev_proc_destroy_buf_device((*destroy_buf_device));

} gx_device_buf_procs_t;

/* Define default buffer device management procedures. */
dev_proc_create_buf_device(gx_default_create_buf_device);
dev_proc_size_buf_device(gx_default_size_buf_device);
dev_proc_setup_buf_device(gx_default_setup_buf_device);
dev_proc_destroy_buf_device(gx_default_destroy_buf_device);

#endif /* gxdevbuf_INCLUDED */
