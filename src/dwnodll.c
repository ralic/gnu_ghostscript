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

/* dwnodll.c */
/* $Id: dwnodll.c,v 1.7 2007/08/01 14:25:43 jemarch Exp $ */

#define STRICT
#include <windows.h>
#include <string.h>
#include <stdio.h>

#include "stdpre.h"
#include "gpgetenv.h"
#include "gscdefs.h"
#define GSREVISION gs_revision

#include "dwdll.h"

/* We are static linking, so just store the function addresses */
int load_dll(GSDLL *gsdll, char *last_error, int len)
{
    gsdll->new_instance = &gsapi_new_instance;
    gsdll->delete_instance = &gsapi_delete_instance;
    gsdll->set_stdio = &gsapi_set_stdio;
    gsdll->set_poll = &gsapi_set_poll;
    gsdll->set_display_callback = &gsapi_set_display_callback;
    gsdll->init_with_args = &gsapi_init_with_args;
    gsdll->run_string = &gsapi_run_string;
    gsdll->exit = &gsapi_exit;
    gsdll->set_visual_tracer = &gsapi_set_visual_tracer;
    return 0;
}

void unload_dll(GSDLL *gsdll)
{
}
