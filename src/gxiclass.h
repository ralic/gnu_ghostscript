/* Copyright (C) 1999 artofcode LLC.  All rights reserved.
  
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

/*$Id: gxiclass.h,v 1.1 2004/01/14 16:59:51 atai Exp $ */
/* Define image rendering algorithm classes */

#ifndef gxiclass_INCLUDED
#  define gxiclass_INCLUDED

/* Define the abstract type for the image enumerator state. */
typedef struct gx_image_enum_s gx_image_enum;

#ifndef gx_device_DEFINED
#  define gx_device_DEFINED
typedef struct gx_device_s gx_device;
#endif

/*
 * Define the interface for routines used to render a (source) scan line.
 * If the buffer is the original client's input data, it may be unaligned;
 * otherwise, it will always be aligned.
 *
 * The image_render procedures work on fully expanded, complete rows.  These
 * take a height argument, which is an integer >= 0; they return a negative
 * code, or the number of rows actually processed (which may be less than
 * the height).  height = 0 is a special call to indicate that there is no
 * more input data; this is necessary because the last scan lines of the
 * source data may not produce any output.
 *
 * Note that the 'w' argument of the image_render procedure is the number
 * of samples, i.e., the number of pixels * the number of samples per pixel.
 * This is neither the width in pixels nor the width in bytes (in the case
 * of 12-bit samples, which expand to 2 bytes apiece).
 */
#define irender_proc(proc)\
  int proc(P6(gx_image_enum *penum, const byte *buffer, int data_x,\
	      uint w, int h, gx_device *dev))
typedef irender_proc((*irender_proc_t));

/*
 * Define procedures for selecting imaging methods according to the class of
 * the image.  Image class procedures are called in alphabetical order, so
 * their names begin with a digit that indicates their priority
 * (0_interpolate, etc.): each one may assume that all the previous ones
 * failed.  If a class procedure succeeds, it may update the enumerator
 * structure as well as returning the rendering procedure.
 */
#define iclass_proc(proc)\
  irender_proc_t proc(P1(gx_image_enum *penum))
typedef iclass_proc((*gx_image_class_t));

#endif /* gxiclass_INCLUDED */
