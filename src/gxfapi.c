/* Copyright (C) 1991, 2000 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gxfapi.c,v 1.3 2005/12/13 16:57:24 jemarch Exp $ */
/* Font API support */

/* GS includes : */
#include "gx.h"
/* UFST includes : */
#include "cgconfig.h"
#include "port.h"
#include "shareinc.h"
/* GS includes : */
#include "gxfapi.h"

/* -------------------- UFST callback dispatcher ------------- */

/*  This code provides dispatching UFST callbacks to GS or PCL. */

struct IF_STATE;

private LPUB8 stub_PCLEO_charptr(LPUB8 pfont_hdr, UW16  sym_code)
{   return NULL;
}

private LPUB8 stub_PCLchId2ptr(IF_STATE *pIFS, UW16 chId)
{   return NULL;
}

private LPUB8 stub_PCLglyphID2Ptr(IF_STATE *pIFS, UW16 glyphID)
{   return NULL;
}

/*
    The following 3 variables are defined statically until the
    reentrancy of graphics library is fixed.
    Also we are waiting for Agfa to fix the reentrancy of UFST callback PCLEO_charptr.
 */

private LPUB8 (*m_PCLEO_charptr)(LPUB8 pfont_hdr, UW16  sym_code) = stub_PCLEO_charptr;
private LPUB8 (*m_PCLchId2ptr)(IF_STATE *pIFS, UW16 chId) = stub_PCLchId2ptr;
private LPUB8 (*m_PCLglyphID2Ptr)(IF_STATE *pIFS, UW16 glyphID) = stub_PCLglyphID2Ptr;


LPUB8 PCLEO_charptr(LPUB8 pfont_hdr, UW16  sym_code)
{   return m_PCLEO_charptr(pfont_hdr, sym_code);
}

LPUB8 PCLchId2ptr(IF_STATE *pIFS, UW16 chId)
{   return m_PCLchId2ptr(FSA chId);
}

LPUB8 PCLglyphID2Ptr(IF_STATE *pIFS, UW16 glyphID)
{   return m_PCLglyphID2Ptr(FSA glyphID);
}

void gx_set_UFST_Callbacks(LPUB8 (*p_PCLEO_charptr)(LPUB8 pfont_hdr, UW16  sym_code),
                           LPUB8 (*p_PCLchId2ptr)(IF_STATE *pIFS, UW16 chId),
                           LPUB8 (*p_PCLglyphID2Ptr)(IF_STATE *pIFS, UW16 glyphID))
{   m_PCLEO_charptr = p_PCLEO_charptr;
    m_PCLchId2ptr = p_PCLchId2ptr;
    m_PCLglyphID2Ptr = p_PCLglyphID2Ptr;
}

void gx_reset_UFST_Callbacks()
{   m_PCLEO_charptr = stub_PCLEO_charptr;
    m_PCLchId2ptr = stub_PCLchId2ptr;
    m_PCLglyphID2Ptr = stub_PCLglyphID2Ptr;
}
