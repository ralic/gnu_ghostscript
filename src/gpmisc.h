/* Copyright (C) 2000 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gpmisc.h,v 1.3 2005/04/18 12:06:03 Arabidopsis Exp $ */
/* Miscellaneous support for platform facilities */

#ifndef gpmisc_INCLUDED
#  define gpmisc_INCLUDED

/*
 * The facilities defined in this file and implemented in gpmisc.c are
 * generic utilities shared among multiple gp_ platform files.
 */

/*
 * Get the name of the directory for temporary files, if any.  Currently
 * this checks the TMPDIR and TEMP environment variables, in that order.
 * The return value and the setting of *ptr and *plen are as for gp_getenv.
 */
int gp_gettmpdir(char *ptr, int *plen);

/*
 * Open a temporary file, using O_EXCL and S_IRWXU to prevent race
 * conditions and symlink attacks.
 */
FILE *gp_fopentemp(const char *fname, const char *mode);

/*
 * Combine a file name with a prefix.
 * Concatenates two paths and reduce parent references and current 
 * directory references from the concatenation when possible.
 * The trailing zero byte is being added.
 */
gp_file_name_combine_result gp_file_name_combine_generic(const char *prefix, uint plen, 
	    const char *fname, uint flen, bool no_sibling, char *buffer, uint *blen);

/*
 * Reduces parent references and current directory references when possible.
 * The trailing zero byte is being added.
 */
gp_file_name_combine_result gp_file_name_reduce(const char *fname, uint flen, 
		char *buffer, uint *blen);

/* 
 * Answers whether a file name is absolute (starts from a root). 
 */
bool gp_file_name_is_absolute(const char *fname, uint flen);

/* 
 * Returns length of all starting parent references.
 */
uint gp_file_name_parents(const char *fname, uint flen);

/* 
 * Returns length of all starting cwd references.
 */
uint gp_file_name_cwds(const char *fname, uint flen);

#endif /* gpmisc_INCLUDED */
