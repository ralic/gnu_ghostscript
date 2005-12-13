/* Copyright (C) 1995, 1998 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: icfontab.c,v 1.4 2005/12/13 16:57:25 jemarch Exp $ */
/* Table of compiled fonts */
#include "ccfont.h"

/*
 * This is compiled separately and linked with the fonts themselves,
 * in a shared library when applicable.
 */

#undef font_
#define font_(fname, fproc, zfproc) extern ccfont_proc(fproc);
#ifndef GCONFIGF_H
# include "gconfigf.h"
#else
# include GCONFIGF_H
#endif

private const ccfont_fproc fprocs[] = {
#undef font_
#define font_(fname, fproc, zfproc) fproc,  /* fname, zfproc are not needed */
#ifndef GCONFIGF_H
# include "gconfigf.h"
#else
# include GCONFIGF_H
#endif
    0
};

int
ccfont_fprocs(int *pnum_fprocs, const ccfont_fproc ** pfprocs)
{
    *pnum_fprocs = countof(fprocs) - 1;
    *pfprocs = &fprocs[0];
    return ccfont_version;	/* for compatibility checking */
}
