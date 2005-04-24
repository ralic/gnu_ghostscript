#!/usr/bin/env python
# -*- Mode: python -*-

#    Copyright (C) 2001 Artifex Software Inc.
# 
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License version 2
#  as published by the Free Software Foundation.
#
#
#  This software is provided AS-IS with no warranty, either express or
#  implied. That is, this program is distributed in the hope that it will 
#  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  59 Temple Place, Suite 330, Boston, MA, 02111-1307.
# 
# For more information about licensing, please refer to
# http://www.ghostscript.com/licensing/. For information on
# commercial licensing, go to http://www.artifex.com/licensing/ or
# contact Artifex Software, Inc., 101 Lucas Valley Road #110,
# San Rafael, CA  94903, U.S.A., +1(415)492-9861.

# $Id: gstestgs.py,v 1.2 2005/04/18 12:06:13 Arabidopsis Exp $

# gstestgs.py
#
# base classes for regression testing

import os
import string
import gsconf
from gstestutils import GSTestCase

class Ghostscript:
	def __init__(self):
		self.command = '/usr/bin/gs'
		self.dpi = 72
		self.band = 0
		self.device = ''
		self.infile = 'input'
		if os.name == 'nt':
			self.nullfile = 'nul'
		else:
			self.nullfile = '/dev/null'
		self.outfile = self.nullfile

		# log file options
		# NOTE: we always append to the log.  if it is desired to start a new
		# log, it is the responsibility of the caller to clear/erase the old
		# one.
		self.log_stdout = self.nullfile
		self.log_stderr = self.nullfile

	def process(self):
		bandsize = 30000000
		if (self.band): bandsize = 10000
		
		cmd = self.command
		cmd = cmd + ' -dQUIET -dNOPAUSE -dBATCH -K1000000 '
		if self.dpi:
			cmd = cmd + '-r%d ' % (self.dpi,)
		cmd = cmd + '-dMaxBitmap=%d ' % (bandsize,)
		cmd = cmd + '-sDEVICE=%s ' % (self.device,)
		cmd = cmd + '-sOutputFile=%s ' % (self.outfile,)

		# as of gs_init 1.93, job server emulation needs -dNOOUTERSAVE
		# so that the 'exitserver' will restore global VM as expected.
		# As of gs_init 1.87.2.9, released with Ghostscript 8.15,
		# job server emulation is supported (in a backward compatible
		# fashion) so we add -dJOBSERVER. The old options are 
		# kept so tests will run against older releases.
		cmd = cmd + '-dNOOUTERSAVE -dJOBSERVER -c false 0 startjob pop '

		if string.lower(self.infile[-4:]) == ".pdf":
			cmd = cmd + ' -dFirstPage=1 -dLastPage=1 '
		else:
			cmd = cmd + '- < '

		cmd = cmd + ' %s >> %s 2>> %s' % (self.infile, self.log_stdout, self.log_stderr)


		# before we execute the command which might append to the log
		# we output a short header to show the commandline that generates
		# the log entry.
		if len(self.log_stdout) > 0 and self.log_stdout != self.nullfile:
			try:
				log = open(self.log_stdout, "a")
				log.write("===\n%s\n---\n" % (cmd,))
				log.close()
			except:
				pass
		if len(self.log_stderr) > 0 and self.log_stderr != self.nullfile:
			try:
				log = open(self.log_stderr, "a")
				log.write("===\n%s\n---\n" % (cmd,))
				log.close()
			except:
				pass
			

		ret = os.system(cmd)

		if ret == 0:
			return 1
		else:
			return 0

		
class GhostscriptTestCase(GSTestCase):
	def __init__(self, gs='gs', dpi=72, band=0, file='test.ps', device='pdfwrite', log_stdout='', log_stderr='', track_daily=0):
		self.gs = gs
		self.file = file
		self.dpi = dpi
		self.band = band
		self.device = device
		self.log_stdout = log_stdout
		self.log_stderr = log_stderr
		self.track_daily = track_daily
		GSTestCase.__init__(self)


class GSCrashTestCase(GhostscriptTestCase):
	def runTest(self):
		gs = Ghostscript()
		gs.command = self.gs
		gs.dpi = self.dpi
		gs.band = self.band
		gs.device = self.device
		gs.infile = self.file

		self.assert_(gs.process(), 'ghostscript failed to render file: ' + self.file)
