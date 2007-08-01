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
/* $Id: gdevmpla.h,v 1.6 2007/08/01 14:25:50 jemarch Exp $ */
/* Interface to planar memory devices. */

#ifndef gdevmpla_INCLUDED
#  define gdevmpla_INCLUDED

/*
 * Planar memory devices store the bits by planes instead of by chunks.
 * The plane corresponding to the least significant bits of the color index
 * is stored first.  Each plane may store a different number of bits,
 * but the depth of each plane must be an allowable one for a memory
 * device and not greater than 16 (currently, 1, 2, 4, 8, or 16), and the
 * total must not exceed the size of gx_color_index.
 *
 * Planar devices store the data for each plane contiguously, as though
 * each plane were a separate device.  There is an array of line pointers
 * for each plane (num_planes arrays in all).
 */

/*
 * Set up a planar memory device, after calling gs_make_mem_device but
 * before opening the device.  The pre-existing device provides the color
 * mapping procedures, but not the drawing procedures.  Requires: num_planes
 * > 0, plane_depths[0 ..  num_planes - 1] > 0, sum of plane_depths <=
 * mdev->color_info.depth.
 */
int gdev_mem_set_planar(gx_device_memory * mdev, int num_planes,
			const gx_render_plane_t *planes /*[num_planes]*/);

#endif /* gdevmpla_INCLUDED */
