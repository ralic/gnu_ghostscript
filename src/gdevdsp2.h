/* Copyright (C) 2001, Ghostgum Software Pty Ltd.  All rights reserved.

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

/* $Id: gdevdsp2.h,v 1.4 2005/12/13 16:57:18 jemarch Exp $ */
/* gdevdsp2.c */

#ifndef gdevdsp2_INCLUDED
#  define gdevdsp2_INCLUDED

typedef struct gx_device_display_s gx_device_display;

#define gx_device_display_common\
	gx_device_memory *mdev;\
	display_callback *callback;\
	void *pHandle;\
	int nFormat;\
	void *pBitmap;\
	unsigned long ulBitmapSize

/* The device descriptor */
struct gx_device_display_s {
    gx_device_common;
    gx_device_display_common;
};

extern_st(st_device_display);
#define public_st_device_display()	/* in gdevdsp.c */\
  gs_public_st_composite_use_final(st_device_display, gx_device_display,\
    "gx_device_display", display_enum_ptrs, display_reloc_ptrs,\
    gx_device_finalize)

#endif /* gdevdsp2_INCLUDED */
