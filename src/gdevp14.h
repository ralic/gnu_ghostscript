/* Copyright (C) 2001 artofcode LLC.  All rights reserved.
  
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

/* $Id: gdevp14.h,v 1.4 2005/12/13 16:57:18 jemarch Exp $ */
/* Definitions and interface for PDF 1.4 rendering device */

#ifndef gdevp14_INCLUDED
#  define gdevp14_INCLUDED

int
gs_pdf14_device_filter(gs_device_filter_t **pdf, int depth, gs_memory_t *mem);

#endif /* gdevp14_INCLUDED */
