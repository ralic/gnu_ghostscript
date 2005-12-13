/* Copyright (C) 1993, 1998, 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: windows_.h,v 1.5 2005/12/13 16:57:28 jemarch Exp $ */
/* Wrapper for windows.h */

#ifndef windows__INCLUDED
#  define windows__INCLUDED

#define STRICT
#include <windows.h>

#ifdef __WATCOMC__
typedef RGBQUAD FAR * LPRGBQUAD;
	/* Watcom's _beginthread takes an extra stack_bottom argument. */
#  define BEGIN_THREAD(proc, stksize, data)\
     _beginthread(proc, NULL, stksize, data)
#else
#  define BEGIN_THREAD(proc, stksize, data)\
     _beginthread(proc, stksize, data)
	/* Define null equivalents of the Watcom 32-to-16-bit glue. */
#  define AllocAlias16(ptr) ((DWORD)(ptr))
#  define FreeAlias16(dword)	/* */
#  define MK_FP16(fp32) ((DWORD)(fp32))
#  define MK_FP32(fp16) (fp16)
#  define GetProc16(proc, ptype) (proc)
#  define ReleaseProc16(cbp)	/* */
#endif

/* Substitute for special "far" library procedures under Win32. */
#ifdef __WIN32__
#  undef _fstrtok
#  define _fstrtok(str, set) strtok(str, set)
#endif

#endif /* windows__INCLUDED */
