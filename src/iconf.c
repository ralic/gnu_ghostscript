/* Copyright (C) 1995, 1996, 1997, 1998, 1999, 2001 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: iconf.c,v 1.2 2004/02/14 22:20:19 atai Exp $ */
/* Configuration-dependent tables and initialization for interpreter */
#include "stdio_.h"		/* stdio for stream.h */
#include "gstypes.h"
#include "gsmemory.h"		/* for iminst.h */
#include "gconf.h"
#include "iref.h"
#include "ivmspace.h"
#include "opdef.h"
#include "ifunc.h"
#include "iapi.h"
#include "iminst.h"
#include "iplugin.h"

/* Define the default values for an interpreter instance. */
const gs_main_instance gs_main_instance_init_values =
{gs_main_instance_default_init_values};

/* Set up the .ps file name string array. */
/* We fill in the lengths at initialization time. */
#define ref_(t) struct { struct tas_s tas; t value; }
#define string_(s,len)\
 { { (t_string<<r_type_shift) + a_readonly + avm_foreign, len }, s },
#define psfile_(fns,len) string_(fns,len)
const ref_(const char *) gs_init_file_array[] = {
#include "gconf.h"
    string_(0, 0)
};
#undef psfile_

/* Set up the emulator name string array similarly. */
#define emulator_(ems,len) string_(ems,len)
const ref_(const char *) gs_emulator_name_array[] = {
#include "gconf.h"
    string_(0, 0)
};
#undef emulator_

/* Set up the function type table similarly. */
#define function_type_(i,proc) extern build_function_proc(proc);
#include "gconf.h"
#undef function_type_
#define function_type_(i,proc) {i,proc},
const build_function_type_t build_function_type_table[] = {
#include "gconf.h"
    {0}
};
#undef function_type_
const uint build_function_type_table_count =
    countof(build_function_type_table) - 1;

/* Initialize the operators. */
	/* Declare the externs. */
#define oper_(xx_op_defs) extern const op_def xx_op_defs[];
oper_(interp_op_defs)		/* Interpreter operators */
#include "gconf.h"
#undef oper_
 
const op_def *const op_defs_all[] = {
#define oper_(defs) defs,
    oper_(interp_op_defs)	/* Interpreter operators */
#include "gconf.h"
#undef oper_ 
    0
};
const uint op_def_count = (countof(op_defs_all) - 1) * OP_DEFS_MAX_SIZE;

/* Set up the plugin table. */

#define plugin_(proc) extern plugin_instantiation_proc(proc);
#include "gconf.h"
#undef plugin_

extern_i_plugin_table();
#define plugin_(proc) proc,
const i_plugin_instantiation_proc i_plugin_table[] = {
#include "gconf.h"
    0
};
#undef plugin_
