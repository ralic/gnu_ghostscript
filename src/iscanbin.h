/* Copyright (C) 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: iscanbin.h,v 1.4 2005/12/13 16:57:26 jemarch Exp $ */
/* Interface to binary token scanner */

#ifndef iscanbin_INCLUDED
#  define iscanbin_INCLUDED

/*
 * Scan a binary token.  The main scanner calls this iff recognize_btokens()
 * is true.  Return e_unregistered if Level 2 features are not included.
 * Return 0 or scan_BOS on success, <0 on failure.
 *
 * This header file exists only because there are two implementations of
 * this procedure: a dummy one for Level 1 systems, and the real one.
 * The interface is entirely internal to the scanner.
 */
int scan_binary_token(i_ctx_t *i_ctx_p, stream *s, ref *pref,
		      scanner_state *pstate);

#endif /* iscanbin_INCLUDED */
