/* Copyright (C) 1996, 2001, Ghostgum Software Pty Ltd.  All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License version 2
  as published by the Free Software Foundation.


  This software is provided AS-IS with no warranty, either express or
  implied. That is, this program is distributed in the hope that it will 
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place, Suite 330, Boston, MA, 02111-1307.
  
  For more information about licensing, please refer to
  http://www.ghostscript.com/licensing/. For information on
  commercial licensing, go to http://www.artifex.com/licensing/ or
  contact Artifex Software, Inc., 101 Lucas Valley Road #110,
  San Rafael, CA  94903, U.S.A., +1(415)492-9861.
 */


/* $Id: dwdll.h,v 1.2 2004/02/14 22:20:05 atai Exp $*/

/* gsdll structure for MS-Windows */

#ifndef dwdll_INCLUDED
#  define dwdll_INCLUDED

#ifndef __PROTOTYPES__
#define __PROTOTYPES__
#endif

#include "iapi.h"

typedef struct GSDLL_S {
	HINSTANCE hmodule;	/* DLL module handle */
	PFN_gsapi_revision revision;
	PFN_gsapi_new_instance new_instance;
	PFN_gsapi_delete_instance delete_instance;
	PFN_gsapi_set_stdio set_stdio;
	PFN_gsapi_set_poll set_poll;
	PFN_gsapi_set_display_callback set_display_callback;
	PFN_gsapi_init_with_args init_with_args;
	PFN_gsapi_run_string run_string;
	PFN_gsapi_exit exit;
        PFN_gsapi_set_visual_tracer set_visual_tracer;
} GSDLL;

/* Load the Ghostscript DLL.
 * Return 0 on success.
 * Return non-zero on error and store error message 
 * to last_error of length len
 */
int load_dll(GSDLL *gsdll, char *last_error, int len);

void unload_dll(GSDLL *gsdll);

#endif /* dwdll_INCLUDED */
