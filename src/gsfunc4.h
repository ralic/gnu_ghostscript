/* Copyright (C) 2000 artofcode LLC.  All rights reserved.
  
  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place, Suite 330, Boston, MA, 02111-1307.

*/

/*$Id: gsfunc4.h,v 1.1 2004/01/14 16:59:48 atai Exp $ */
/* Definitions for "PostScript Calculator" Functions */

#ifndef gsfunc4_INCLUDED
#  define gsfunc4_INCLUDED

#include "gsfunc.h"

/* ---------------- Types and structures ---------------- */

/* Define the Function type. */
#define function_type_PostScript_Calculator 4

/* Define the opcodes. */
typedef enum {

    /* Arithmetic operators */

    PtCr_abs, PtCr_add, PtCr_and, PtCr_atan, PtCr_bitshift,
    PtCr_ceiling, PtCr_cos, PtCr_cvi, PtCr_cvr, PtCr_div, PtCr_exp,
    PtCr_floor, PtCr_idiv, PtCr_ln, PtCr_log, PtCr_mod, PtCr_mul,
    PtCr_neg, PtCr_not, PtCr_or, PtCr_round,
    PtCr_sin, PtCr_sqrt, PtCr_sub, PtCr_truncate, PtCr_xor,

    /* Comparison operators */

    PtCr_eq, PtCr_ge, PtCr_gt, PtCr_le, PtCr_lt, PtCr_ne,

    /* Stack operators */

    PtCr_copy, PtCr_dup, PtCr_exch, PtCr_index, PtCr_pop, PtCr_roll,

    /* Constants */

    PtCr_byte, PtCr_int /* native */, PtCr_float /* native */,
    PtCr_true, PtCr_false,

    /* Special operators */

    PtCr_if, PtCr_else, PtCr_return

} gs_PtCr_opcode_t;
#define PtCr_NUM_OPS ((int)PtCr_byte)
#define PtCr_NUM_OPCODES ((int)PtCr_return + 1)

/* Define PostScript Calculator functions. */
typedef struct gs_function_PtCr_params_s {
    gs_function_params_common;
    gs_const_string ops;	/* gs_PtCr_opcode_t[] */
} gs_function_PtCr_params_t;

/****** NEEDS TO INCLUDE data_source ******/
#define private_st_function_PtCr()	/* in gsfunc4.c */\
  gs_private_st_suffix_add_strings1(st_function_PtCr, gs_function_PtCr_t,\
    "gs_function_PtCr_t", function_PtCr_enum_ptrs, function_PtCr_reloc_ptrs,\
    st_function, params.ops)

/* ---------------- Procedures ---------------- */

/* Allocate and initialize a PostScript Calculator function. */
int gs_function_PtCr_init(P3(gs_function_t ** ppfn,
			     const gs_function_PtCr_params_t * params,
			     gs_memory_t * mem));

/* Free the parameters of a PostScript Calculator function. */
void gs_function_PtCr_free_params(P2(gs_function_PtCr_params_t * params,
				     gs_memory_t * mem));

#endif /* gsfunc4_INCLUDED */
