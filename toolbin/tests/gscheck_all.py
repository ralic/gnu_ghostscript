#!/usr/bin/env python

#    Copyright (C) 2002 Aladdin Enterprises.  All rights reserved.
# 
# This file is part of GNU ghostscript
#
# GNU ghostscript is free software; you can redistribute it and/or
# modify it under the terms of the version 2 of the GNU General Public
# License as published by the Free Software Foundation.
#
# This software is provided AS-IS with no warranty, either express or
# implied. That is, this program is distributed in the hope that it will 
# be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA, 02110-1301.


# $Id: gscheck_all.py,v 1.5 2007/08/01 14:26:57 jemarch Exp $

# Run all the Ghostscript 'gscheck' tests.

from gstestutils import gsRunTestsMain

def addTests(suite, **args):
    import gscheck_raster; gscheck_raster.addTests(suite, **args)
    import gscheck_pdfwrite; gscheck_pdfwrite.addTests(suite, **args)

if __name__ == "__main__":
    gsRunTestsMain(addTests)
