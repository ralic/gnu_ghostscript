/******************************************************************************
  File:     $Id: pagecount.h,v 1.2 2010/07/11 19:47:19 Arabidopsis Exp $
  Contents: Header for pagecount file functions
  Author:   Martin Lottermoser, Greifswaldstrasse 28, 38124 Braunschweig,
            Germany; e-mail: Martin.Lottermoser@t-online.de.

*******************************************************************************
*									      *
*	Copyright (C) 2000 by Martin Lottermoser			      *
*	All rights reserved						      *
*									      *
******************************************************************************/

#ifndef _pagecount_h	/* Inclusion protection */
#define _pagecount_h

/* Configuration management identification */
#pragma ident "@(#)$Id: pagecount.h,v 1.2 2010/07/11 19:47:19 Arabidopsis Exp $"

/*****************************************************************************/

/*  The following two functions are used to read and write a "page count file".
    Such a file should contain a single line with a non-negative decimal
    integer. pcf_getcount() reads the file and returns the value,
    pcf_inccount() increases the number in the file by the specified amount.
    A non-existent file is treated as a file containing the number zero.

    The routines are safe against concurrent access.
*/
extern int pcf_getcount(const char *filename, unsigned long *count);
extern int pcf_inccount(const char *filename, unsigned long by);

/*****************************************************************************/

#endif	/* Inclusion protection */