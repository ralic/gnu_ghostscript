/* Copyright (C) 1993, 1995, 1998, 1999 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: slzwx.h,v 1.2 2004/02/14 22:20:19 atai Exp $ */
/* Definitions for LZW filters */
/* Requires strimpl.h */

#ifndef slzwx_INCLUDED
#  define slzwx_INCLUDED

typedef struct lzw_decode_s lzw_decode;
typedef struct lzw_encode_table_s lzw_encode_table;
typedef struct stream_LZW_state_s {
    stream_state_common;
    /* The following are set before initialization. */
    int InitialCodeLength;	/* decoding only */
    /*
     * Adobe calls FirstBitLowOrder LowBitFirst.  Either one will work
     * in PostScript code.
     */
    bool FirstBitLowOrder;	/* decoding only */
    bool BlockData;		/* decoding only */
    int EarlyChange;		/* decoding only */
    /* The following are updated dynamically. */
    uint bits;			/* buffer for input bits */
    int bits_left;		/* Decode: # of valid bits left, [0..7] */
				/* (low-order bits if !FirstBitLowOrder, */
				/* high-order bits if FirstBitLowOrder) */
    int bytes_left;		/* # of bytes left in current block */
				/* (arbitrary large # if not GIF) */
    union _lzt {
	lzw_decode *decode;
	lzw_encode_table *encode;
    } table;
    uint next_code;		/* next code to be assigned */
    int code_size;		/* current # of bits per code */
    int prev_code;		/* previous code recognized or assigned */
    uint prev_len;		/* length of prev_code */
    int copy_code;		/* code whose string is being */
				/* copied, -1 if none */
    uint copy_len;		/* length of copy_code */
    int copy_left;		/* amount of string left to copy */
    bool first;			/* true if no output yet */
} stream_LZW_state;

extern_st(st_LZW_state);
#define public_st_LZW_state()	/* in slzwc.c */\
  gs_public_st_ptrs1(st_LZW_state, stream_LZW_state,\
    "LZWDecode state", lzwd_enum_ptrs, lzwd_reloc_ptrs, table.decode)
#define s_LZW_set_defaults_inline(ss)\
  ((ss)->InitialCodeLength = 8,\
   (ss)->FirstBitLowOrder = false,\
   (ss)->BlockData = false,\
   (ss)->EarlyChange = 1,\
   /* Clear pointers */\
   (ss)->table.decode /*=encode*/ = 0)
extern const stream_template s_LZWD_template;
extern const stream_template s_LZWE_template;

/* Shared procedures */
void s_LZW_set_defaults(stream_state *);
void s_LZW_release(stream_state *);

#endif /* slzwx_INCLUDED */