/* Copyright (C) 1998 Aladdin Enterprises.  All rights reserved.
  
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

/* $Id: gdevmeds.c,v 1.4 2005/12/13 16:57:18 jemarch Exp $ */
/*
 * Media selection support for printer drivers
 *
 * Select from a NULL terminated list of media the smallest medium which is
 * almost equal or larger then the actual imagesize.
 *
 * Written by Ulrich Schmid, uschmid@mail.hh.provi.de.
 */

#include "gdevmeds.h"

#define CM        * 0.01
#define INCH      * 0.0254
#define TOLERANCE 0.1 CM

private const struct {
	const char* name;
	float       width;
	float       height;
	float       priority;
} media[] = {
#define X(name, width, height) {name, width, height, 1 / (width * height)}
	X("a0",           83.9611 CM,   118.816 CM),
	X("a1",           59.4078 CM,   83.9611 CM),
	X("a2",           41.9806 CM,   59.4078 CM),
	X("a3",           29.7039 CM,   41.9806 CM),
	X("a4",           20.9903 CM,   29.7039 CM),
	X("a5",           14.8519 CM,   20.9903 CM),
	X("a6",           10.4775 CM,   14.8519 CM),
	X("a7",           7.40833 CM,   10.4775 CM),
	X("a8",           5.22111 CM,   7.40833 CM),
	X("a9",           3.70417 CM,   5.22111 CM),
	X("a10",          2.61056 CM,   3.70417 CM),
	X("archA",        9 INCH,       12 INCH),
	X("archB",        12 INCH,      18 INCH),
	X("archC",        18 INCH,      24 INCH),
	X("archD",        24 INCH,      36 INCH),
	X("archE",        36 INCH,      48 INCH),
	X("b0",           100.048 CM,   141.393 CM),
	X("b1",           70.6967 CM,   100.048 CM),
	X("b2",           50.0239 CM,   70.6967 CM),
	X("b3",           35.3483 CM,   50.0239 CM),
	X("b4",           25.0119 CM,   35.3483 CM),
	X("b5",           17.6742 CM,   25.0119 CM),
	X("flsa",         8.5 INCH,     13 INCH),
	X("flse",         8.5 INCH,     13 INCH),
	X("halfletter",   5.5 INCH,     8.5 INCH),
	X("ledger",       17 INCH,      11 INCH),
	X("legal",        8.5 INCH,     14 INCH),
	X("letter",       8.5 INCH,     11 INCH),
	X("note",         7.5 INCH,     10 INCH),
	X("executive",    7.25 INCH,    10.5 INCH),
	X("com10",        4.125 INCH,   9.5 INCH),
	X("dl",           11 CM,        22 CM),
	X("c5",           16.2 CM,      22.9 CM),
	X("monarch",      3.875 INCH,   7.5 INCH)};

int select_medium(gx_device_printer *pdev, const char **available, int default_index)
{
	int i, j, index = default_index;
	float priority = 0;
	float width = pdev->width / pdev->x_pixels_per_inch INCH;
	float height = pdev->height / pdev->y_pixels_per_inch INCH;

	for (i = 0; available[i]; i++) {
		for (j = 0; j < sizeof(media) / sizeof(media[0]); j++) {
			if (!strcmp(available[i], media[j].name) &&
			    media[j].width + TOLERANCE > width &&
			    media[j].height + TOLERANCE > height &&
			    media[j].priority > priority) {
				index = i;
				priority = media[j].priority;
			}
		}
	}
	return index;
}
