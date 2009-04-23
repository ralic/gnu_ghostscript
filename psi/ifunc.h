/* Copyright (C) 2001-2006 Artifex Software, Inc.
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

/* $Id: ifunc.h,v 1.1 2009/04/23 23:31:37 Arabidopsis Exp $ */
/* Internal interpreter interfaces for Functions */

#ifndef ifunc_INCLUDED
#  define ifunc_INCLUDED

#include "gsfunc.h"

/* Define build procedures for the various function types. */
#define build_function_proc(proc)\
  int proc(i_ctx_t *i_ctx_p, const ref *op, const gs_function_params_t *params, int depth,\
	   gs_function_t **ppfn, gs_memory_t *mem)
typedef build_function_proc((*build_function_proc_t));

/* Define the table of build procedures, indexed by FunctionType. */
typedef struct build_function_type_s {
    int type;
    build_function_proc_t proc;
} build_function_type_t;
extern const build_function_type_t build_function_type_table[];
extern const uint build_function_type_table_count;

/* Build a function structure from a PostScript dictionary. */
int fn_build_function(i_ctx_t *i_ctx_p, const ref * op, gs_function_t ** ppfn,
      gs_memory_t *mem, const float *shading_domain, const int num_inputs);
int fn_build_sub_function(i_ctx_t *i_ctx_p, const ref * op, gs_function_t ** ppfn,
  int depth, gs_memory_t *mem, const float *shading_domain, const int num_inputs);

/* Called only from the routines in zcolor.c, in order to convert a tint-transform
 * procedure to a function. Little more than a place-holder to avoid making a number
 * of functions non-static
 */
int buildfunction(i_ctx_t * i_ctx_p, ref *arr, ref *pproc, int type);

/*
 * Collect a heap-allocated array of floats.  If the key is missing, set
 * *pparray = 0 and return 0; otherwise set *pparray and return the number
 * of elements.  Note that 0-length arrays are acceptable, so if the value
 * returned is 0, the caller must check whether *pparray == 0.
 */
int fn_build_float_array(const ref * op, const char *kstr, bool required,
			 bool even, const float **pparray,
			 gs_memory_t *mem);

/*
 * Similar to fn_build_float_array() except
 * - numeric parameter is accepted and converted to 1-element array
 * - number of elements is not checked for even/odd
 */
int
fn_build_float_array_forced(const ref * op, const char *kstr, bool required,
		     const float **pparray, gs_memory_t *mem);


/*
 * If a PostScript object is a Function procedure, return the function
 * object, otherwise return 0.
 */
gs_function_t *ref_function(const ref *op);

/*
 * Operator to execute a function.
 * <in1> ... <function_struct> %execfunction <out1> ...
 */
int zexecfunction(i_ctx_t *);

#endif /* ifunc_INCLUDED */
