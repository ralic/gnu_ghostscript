/* Copyright (C) 1997, 1998, 2000 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gdevpxen.h,v 1.2 2004/02/14 22:20:06 atai Exp $ */
/* Enumerated attribute value definitions for PCL XL */

#ifndef gdevpxen_INCLUDED
#  define gdevpxen_INCLUDED

typedef enum {
    eClockWise = 0,
    eCounterClockWise,
    pxeArcDirection_next
} pxeArcDirection_t;

typedef enum {
    eNoSubstitution = 0,
    eVerticalSubstitution,
    pxeCharSubModeArray_next
} pxeCharSubModeArray_t;

typedef enum {
    eNonZeroWinding = 0,
    eEvenOdd,
    pxeClipMode_next,
    pxeFillMode_next = pxeClipMode_next		/* see pxeFillMode_t below */
} pxeClipMode_t;

typedef enum {
    eInterior = 0,
    eExterior,
    pxeClipRegion_next
} pxeClipRegion_t;

typedef enum {
    e1Bit = 0,
    e4Bit,
    e8Bit,
    pxeColorDepth_next
} pxeColorDepth_t;

typedef enum {
    eCRGB = 5,			/* Note: for this enumeration, 0 is not a valid value */
    pxeColorimetricColorSpace_next
} pxeColorimetricColorSpace_t;	/* 2.0 */

typedef enum {
    eDirectPixel = 0,
    eIndexedPixel,
    pxeColorMapping_next
} pxeColorMapping_t;

typedef enum {
    eNoColorSpace = 0,		/* Note: for this enumeration, 0 is not a valid value */
    eGray,
    eRGB,
    eSRGB = 6,		/* 2.0, Note: HP's value is 6 not the expected 3 */
    pxeColorSpace_next
} pxeColorSpace_t;

typedef enum {
    eNoCompression = 0,
    eRLECompression,
    eJPEGCompression,		/* 2.0 */
    pxeCompressMode_next
} pxeCompressMode_t;

typedef enum {
    eBinaryHighByteFirst = 0,
    eBinaryLowByteFirst,
    pxeDataOrg_next		/* is this DataOrg or DataOrganization? */
} pxeDataOrg_t;

typedef enum {
    eDefault = 0,		/* bad choice of name! */
    pxeDataSource_next
} pxeDataSource_t;

typedef enum {
    eUByte = 0,
    eSByte,
    eUInt16,
    eSInt16,
    pxeDataType_next
} pxeDataType_t;

typedef enum {
    eDownloaded = -1,		/* Not a real value, indicates a downloaded matrix */
    eDeviceBest = 0,
    pxeDitherMatrix_next
} pxeDitherMatrix_t;

typedef enum {
    eDuplexHorizontalBinding = 0,
    eDuplexVerticalBinding,
    pxeDuplexPageMode_next
} pxeDuplexPageMode_t;

typedef enum {
    eFrontMediaSide = 0,
    eBackMediaSide,
    pxeDuplexPageSide_next
} pxeDuplexPageSide_t;

typedef enum {
    /* Several pieces of code know that this is a bit mask. */
    eNoReporting = 0,
    eBackChannel,
    eErrorPage,
    eBackChAndErrPage,
    eNWBackChannel,		/* 2.0 */
    eNWErrorPage,		/* 2.0 */
    eNWBackChAndErrPage,	/* 2.0 */
    pxeErrorReport_next
} pxeErrorReport_t;

typedef pxeClipMode_t pxeFillMode_t;

typedef enum {
    eButtCap = 0,
    eRoundCap,
    eSquareCap,
    eTriangleCap,
    pxeLineCap_next
} pxeLineCap_t;

#define pxeLineCap_to_library\
  { gs_cap_butt, gs_cap_round, gs_cap_square, gs_cap_triangle }

typedef enum {
    eMiterJoin = 0,
    eRoundJoin,
    eBevelJoin,
    eNoJoin,
    pxeLineJoin_next
} pxeLineJoin_t;

#define pxeLineJoin_to_library\
  { gs_join_miter, gs_join_round, gs_join_bevel, gs_join_none }

typedef enum {
    eInch = 0,
    eMillimeter,
    eTenthsOfAMillimeter,
    pxeMeasure_next
} pxeMeasure_t;

#define pxeMeasure_to_points { 72.0, 72.0 / 25.4, 72.0 / 254.0 }

typedef enum {
    eDefaultDestination = 0,
    eFaceDownBin,		/* 2.0 */
    eFaceUpBin,			/* 2.0 */
    eJobOffsetBin,		/* 2.0 */
    pxeMediaDestination_next
} pxeMediaDestination_t;

typedef enum {
    eLetterPaper = 0,
    eLegalPaper,
    eA4Paper,
    eExecPaper,
    eLedgerPaper,
    eA3Paper,
    eCOM10Envelope,
    eMonarchEnvelope,
    eC5Envelope,
    eDLEnvelope,
    eJB4Paper,
    eJB5Paper,
    eB5Envelope,
    eJPostcard,
    eJDoublePostcard,
    eA5Paper,
    eA6Paper,			/* 2.0 */
    eJB6Paper,			/* 2.0 */
    pxeMediaSize_next
} pxeMediaSize_t;

/*
 * Apply a macro (or procedure) to all known paper sizes.
 * The arguments are:
 *      media size code, resolution for width/height, width, height.
 */
#define px_enumerate_media(m)\
  m(eLetterPaper, 300, 2550, 3300)\
  m(eLegalPaper, 300, 2550, 5300)\
  m(eA4Paper, 300, 2480, 3507)\
  m(eExecPaper, 300, 2175, 3150)\
  m(eLedgerPaper, 300, 3300, 5100)\
  m(eA3Paper, 300, 3507, 4960)\
  m(eCOM10Envelope, 300, 1237, 2850)\
  m(eMonarchEnvelope, 300, 1162, 2250)\
  m(eC5Envelope, 300, 1913, 2704)\
  m(eDLEnvelope, 300, 1299, 2598)\
  m(eB5Envelope, 300, 2078, 2952)

typedef enum {
    eDefaultSource = 0,
    eAutoSelect,
    eManualFeed,
    eMultiPurposeTray,
    eUpperCassette,
    eLowerCassette,
    eEnvelopeTray,
    eThirdCassette,		/* 2.0 */
    pxeMediaSource_next
} pxeMediaSource_t;

/**** MediaType is not documented. ****/
typedef enum {
    eDefaultType = 0,
    pxeMediaType_next
} pxeMediaType_t;

typedef enum {
    ePortraitOrientation = 0,
    eLandscapeOrientation,
    eReversePortrait,
    eReverseLandscape,
    pxeOrientation_next
} pxeOrientation_t;

typedef enum {
    eTempPattern = 0,
    ePagePattern,
    eSessionPattern,
    pxePatternPersistence_next
} pxePatternPersistence_t;

typedef enum {
    eSimplexFrontSide = 0,
    pxeSimplexPageMode_next
} pxeSimplexPageMode_t;

typedef enum {
    eOpaque = 0,
    eTransparent,
    pxeTxMode_next
} pxeTxMode_t;

typedef enum {
    eHorizontal = 0,
    eVertical,
    pxeWritingMode_next
} pxeWritingMode_t;		/* 2.0 */

#endif /* gdevpxen_INCLUDED */
