/* Copyright (C) 1994, 1995, 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: zsysvm.c,v 1.4 2005/12/13 16:57:28 jemarch Exp $ */
/* System VM and VM-specific operators */
#include "ghost.h"
#include "oper.h"
#include "ialloc.h"
#include "ivmspace.h"
#include "store.h"		/* for make_bool */

/*
 * These operators allow creation of objects in a specific VM --
 * local, global, or system.  System VM, which is not a standard PostScript
 * facility, is not subject to save and restore; objects in system VM
 * may only refer to simple objects or to other (composite) objects
 * in system VM.
 */

/* Execute an operator with a specific VM selected as current VM. */
private int
specific_vm_op(i_ctx_t *i_ctx_p, op_proc_t opproc, uint space)
{
    uint save_space = icurrent_space;
    int code;

    ialloc_set_space(idmemory, space);
    code = opproc(i_ctx_p);
    ialloc_set_space(idmemory, save_space);
    return code;
}

/* <int> .globalvmarray <array> */
private int
zglobalvmarray(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zarray, avm_global);
}

/* <int> .globalvmdict <dict> */
private int
zglobalvmdict(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zdict, avm_global);
}

/* <obj_0> ... <obj_n-1> <n> .globalvmpackedarray <packedarray> */
private int
zglobalvmpackedarray(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zpackedarray, avm_global);
}

/* <int> .globalvmstring <string> */
private int
zglobalvmstring(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zstring, avm_global);
}

/* <int> .localvmarray <array> */
private int
zlocalvmarray(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zarray, avm_local);
}

/* <int> .localvmdict <dict> */
private int
zlocalvmdict(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zdict, avm_local);
}

/* <obj_0> ... <obj_n-1> <n> .localvmpackedarray <packedarray> */
private int
zlocalvmpackedarray(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zpackedarray, avm_local);
}

/* <int> .localvmstring <string> */
private int
zlocalvmstring(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zstring, avm_local);
}

/* <int> .systemvmarray <array> */
private int
zsystemvmarray(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zarray, avm_system);
}

/* <int> .systemvmdict <dict> */
private int
zsystemvmdict(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zdict, avm_system);
}

/* <obj_0> ... <obj_n-1> <n> .systemvmpackedarray <packedarray> */
private int
zsystemvmpackedarray(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zpackedarray, avm_system);
}

/* <int> .systemvmstring <string> */
private int
zsystemvmstring(i_ctx_t *i_ctx_p)
{
    return specific_vm_op(i_ctx_p, zstring, avm_system);
}

/* <any> .systemvmcheck <bool> */
private int
zsystemvmcheck(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    make_bool(op, (r_space(op) == avm_system ? true : false));
    return 0;
}

/* ------ Initialization procedure ------ */

const op_def zsysvm_op_defs[] =
{
    {"1.globalvmarray", zglobalvmarray},
    {"1.globalvmdict", zglobalvmdict},
    {"1.globalvmpackedarray", zglobalvmpackedarray},
    {"1.globalvmstring", zglobalvmstring},
    {"1.localvmarray", zlocalvmarray},
    {"1.localvmdict", zlocalvmdict},
    {"1.localvmpackedarray", zlocalvmpackedarray},
    {"1.localvmstring", zlocalvmstring},
    {"1.systemvmarray", zsystemvmarray},
    {"1.systemvmcheck", zsystemvmcheck},
    {"1.systemvmdict", zsystemvmdict},
    {"1.systemvmpackedarray", zsystemvmpackedarray},
    {"1.systemvmstring", zsystemvmstring},
    op_def_end(0)
};
