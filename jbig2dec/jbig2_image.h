/*
    jbig2dec
    
    Copyright (c) 2001-2002 artofcode LLC.
    
    This software is distributed under license and may not
    be copied, modified or distributed except as expressly
    authorized under the terms of the license contained in
    the file LICENSE in this distribution.
                                                                                
    $Id: jbig2_image.h,v 1.3 2006/03/02 21:27:55 Arabidopsis Exp $
*/


#ifndef _JBIG2_IMAGE_H
#define _JBIG2_IMAGE_H

/* routines for dumping the image data in various formats */
/* FIXME: should these be in the client instead? */

int jbig2_image_write_pbm_file(Jbig2Image *image, char *filename);
int jbig2_image_write_pbm(Jbig2Image *image, FILE *out);
Jbig2Image *jbig2_image_read_pbm_file(Jbig2Ctx *ctx, char *filename);
Jbig2Image *jbig2_image_read_pbm(Jbig2Ctx *ctx, FILE *in);

#ifdef HAVE_LIBPNG
int jbig2_image_write_png_file(Jbig2Image *image, char *filename);
int jbig2_image_write_png(Jbig2Image *image, FILE *out);
#endif

int jbig2_image_get_pixel(Jbig2Image *image, int x, int y);
int jbig2_image_set_pixel(Jbig2Image *image, int x, int y, int value);


#endif /* _JBIG2_IMAGE_H */
