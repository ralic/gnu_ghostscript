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
/* $Id: dwimg.h,v 1.7 2007/08/01 14:25:43 jemarch Exp $ */

#ifndef dwimg_INCLUDED
#  define dwimg_INCLUDED


/* Windows Image Window structure */

typedef struct IMAGE_DEVICEN_S IMAGE_DEVICEN;
struct IMAGE_DEVICEN_S {
    int used;		/* non-zero if in use */
    int visible;	/* show on window */
    char name[64];
    int cyan;
    int magenta;
    int yellow;
    int black;
    int menu;		/* non-zero if menu item added to system menu */
};
#define IMAGE_DEVICEN_MAX 8

typedef struct IMAGE_S IMAGE;
struct IMAGE_S {
    void *handle;
    void *device;
    HWND hwnd;
    HBRUSH hBrush;	/* background */
    int raster;
    unsigned int format;
    unsigned char *image;
    BITMAPINFOHEADER bmih;
    HPALETTE palette;
    int bytewidth;
    int devicen_gray;	/* true if a single separation should be shown gray */
    IMAGE_DEVICEN devicen[IMAGE_DEVICEN_MAX];

    /* periodic redrawing */
    UINT update_timer;		/* identifier */
    int update_tick;		/* timer duration in milliseconds */
    int update_count;		/* Number of WM_TIMER messages received */
    int update_interval;	/* Number of WM_TIMER until refresh */
    int pending_update;		/* We have asked for periodic updates */
    int pending_sync;		/* We have asked for a SYNC */

    /* Window scrolling stuff */
    int cxClient, cyClient;
    int cxAdjust, cyAdjust;
    int nVscrollPos, nVscrollMax;
    int nHscrollPos, nHscrollMax;

    /* thread synchronisation */
    HANDLE hmutex;

    IMAGE *next;

    HWND hwndtext;	/* handle to text window */

    int x, y, cx, cy;	/* window position */
};

extern IMAGE *first_image;

/* Main thread only */
IMAGE *image_find(void *handle, void *device);
IMAGE *image_new(void *handle, void *device);
void image_delete(IMAGE *img);
int image_size(IMAGE *img, int new_width, int new_height, int new_raster, 
   unsigned int new_format, void *pimage);

/* GUI thread only */
void image_open(IMAGE *img);
void image_close(IMAGE *img);
void image_sync(IMAGE *img);
void image_page(IMAGE *img);
void image_presize(IMAGE *img, int new_width, int new_height, int new_raster, 
   unsigned int new_format);
void image_poll(IMAGE *img);
void image_updatesize(IMAGE *img);


#endif /* dwimg_INCLUDED */
