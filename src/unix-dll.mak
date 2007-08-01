#  Copyright (C) 2001-2006 artofcode LLC.
#  All Rights Reserved.
#
#  This file is part of GNU ghostscript
#
#  GNU ghostscript is free software; you can redistribute it and/or modify it under
#  the terms of the version 2 of the GNU General Public License as published by the Free Software
#  Foundation.
#
#  GNU ghostscript is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along with
#  ghostscript; see the file COPYING. If not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# $Id: unix-dll.mak,v 1.9 2007/08/01 14:26:47 jemarch Exp $
# Partial makefile for Unix shared library target

# Useful make commands:
#  make so		make ghostscript as a shared object
#  make sodebug		make debug ghostscript as a shared object
#  make soinstall	install shared object ghostscript
#  make soclean		remove build files
#
# If you want to test the executable without installing:
#  export LD_LIBRARY_PATH=/insert-path-here/sobin
#  export GS_LIB=/insert-path-here/lib

# Location for building shared object
SOOBJRELDIR=../soobj
SOBINRELDIR=../sobin

# ------------------- Ghostscript shared object --------------------------- #

# Shared object names

# simple loader (no support for display device)
GSSOC_XENAME=$(GS)c$(XE)
GSSOC_XE=$(BINDIR)/$(GSSOC_XENAME)
GSSOC=$(BINDIR)/$(SOBINRELDIR)/$(GSSOC_XENAME)

# loader suporting display device using Gtk+
GSSOX_XENAME=$(GS)x$(XE)
GSSOX_XE=$(BINDIR)/$(GSSOX_XENAME)
GSSOX=$(BINDIR)/$(SOBINRELDIR)/$(GSSOX_XENAME)

# shared library
GS_SONAME_BASE=lib$(GS)

# GNU/Linux
GS_SOEXT=so
GS_SONAME=$(GS_SONAME_BASE).$(GS_SOEXT)
GS_SONAME_MAJOR=$(GS_SONAME).$(GS_VERSION_MAJOR)
GS_SONAME_MAJOR_MINOR=$(GS_SONAME).$(GS_VERSION_MAJOR).$(GS_VERSION_MINOR)
LDFLAGS_SO=-shared -Wl,-soname=$(GS_SONAME_MAJOR)

# MacOS X
#GS_SOEXT=dylib
#GS_SONAME=$(GS_SONAME_BASE).$(GS_SOEXT)
#GS_SONAME_MAJOR=$(GS_SONAME_BASE).$(GS_VERSION_MAJOR).$(GS_SOEXT)
#GS_SONAME_MAJOR_MINOR=$(GS_SONAME_BASE).$(GS_VERSION_MAJOR).$(GS_VERSION_MINOR).$(GS_SOEXT)
#LDFLAGS_SO=-dynamiclib -flat-namespace
#LDFLAGS_SO=-dynamiclib -install-name $(GS_SONAME_MAJOR_MINOR)

GS_SO=$(BINDIR)/$(GS_SONAME)
GS_SO_MAJOR=$(BINDIR)/$(GS_SONAME_MAJOR)
GS_SO_MAJOR_MINOR=$(BINDIR)/$(GS_SONAME_MAJOR_MINOR)

# Shared object is built by redefining GS_XE in a recursive make.

# Create symbolic links to the Ghostscript interpreter library

$(GS_SO): $(GS_SO_MAJOR)
	$(RM_) $(GS_SO)
	ln -s $(GS_SONAME_MAJOR_MINOR) $(GS_SO)

$(GS_SO_MAJOR): $(GS_SO_MAJOR_MINOR)
	$(RM_) $(GS_SO_MAJOR)
	ln -s $(GS_SONAME_MAJOR_MINOR) $(GS_SO_MAJOR)

# Build the small Ghostscript loaders, with Gtk+ and without

$(GSSOC_XE): $(GS_SO) $(GLSRC)dxmainc.c
	$(GLCC) -g -o $(GSSOC_XE) $(GLSRC)dxmainc.c -L$(BINDIR) -l$(GS)

$(GSSOX_XE): $(GS_SO) $(GLSRC)dxmain.c
	$(GLCC) -g `pkg-config --cflags gtk+-2.0` -o $(GSSOX_XE) $(GLSRC)dxmain.c -L$(BINDIR) -l$(GS) `pkg-config --libs gtk+-2.0`


# ------------------------- Recursive make targets ------------------------- #

SODEFS=LDFLAGS='$(LDFLAGS) $(LDFLAGS_SO)'\
 GS_XE=$(BINDIR)/$(SOBINRELDIR)/$(GS_SONAME_MAJOR_MINOR)\
 STDIO_IMPLEMENTATION=c\
 DISPLAY_DEV=$(DD)$(SOOBJRELDIR)/display.dev\
 BINDIR=$(BINDIR)/$(SOBINRELDIR)\
 GLGENDIR=$(GLGENDIR)/$(SOOBJRELDIR)\
 GLOBJDIR=$(GLOBJDIR)/$(SOOBJRELDIR)\
 PSGENDIR=$(PSGENDIR)/$(SOOBJRELDIR)\
 PSOBJDIR=$(PSOBJDIR)/$(SOOBJRELDIR)


# Normal shared object
so: SODIRS
	$(MAKE) $(SODEFS) CFLAGS='$(CFLAGS_STANDARD) $(CFLAGS_SO) $(GCFLAGS) $(XCFLAGS)' prefix=$(prefix) $(GSSOC) $(GSSOX)

# Debug shared object
# Note that this is in the same directory as the normal shared
# object, so you will need to use 'make soclean', 'make sodebug'
sodebug: SODIRS
	$(MAKE) $(SODEFS) GENOPT='-DDEBUG' CFLAGS='$(CFLAGS_DEBUG) $(CFLAGS_SO) $(GCFLAGS) $(XCFLAGS)' $(GSSOC) $(GSSOX)

install-so: so
	-mkdir -p $(prefix)
	-mkdir -p $(datadir)
	-mkdir -p $(gsdir)
	-mkdir -p $(gsdatadir)
	-mkdir -p $(bindir)
	-mkdir -p $(libdir)
	$(INSTALL_PROGRAM) $(GSSOC) $(bindir)/$(GSSOC_XENAME)
	$(INSTALL_PROGRAM) $(GSSOX) $(bindir)/$(GSSOX_XENAME)
	$(INSTALL_PROGRAM) $(BINDIR)/$(SOBINRELDIR)/$(GS_SONAME_MAJOR_MINOR) $(libdir)/$(GS_SONAME_MAJOR_MINOR)
	$(RM_) $(libdir)/$(GS_SONAME)
	ln -s $(GS_SONAME_MAJOR_MINOR) $(libdir)/$(GS_SONAME)
	$(RM_) $(libdir)/$(GS_SONAME_MAJOR)
	ln -s $(GS_SONAME_MAJOR_MINOR) $(libdir)/$(GS_SONAME_MAJOR)

soinstall: install-so install-scripts install-data

# Make the build directories
SODIRS: STDDIRS
	@if test ! -d $(BINDIR)/$(SOBINRELDIR); then mkdir -p $(BINDIR)/$(SOBINRELDIR); fi
	@if test ! -d $(GLGENDIR)/$(SOOBJRELDIR); then mkdir -p $(GLGENDIR)/$(SOOBJRELDIR); fi
	@if test ! -d $(GLOBJDIR)/$(SOOBJRELDIR); then mkdir -p $(GLOBJDIR)/$(SOOBJRELDIR); fi
	@if test ! -d $(PSGENDIR)/$(SOOBJRELDIR); then mkdir -p $(PSGENDIR)/$(SOOBJRELDIR); fi
	@if test ! -d $(PSOBJDIR)/$(SOOBJRELDIR); then mkdir -p $(PSOBJDIR)/$(SOOBJRELDIR); fi


soclean: SODIRS
	$(MAKE) $(SODEFS) clean
	$(RM_) $(BINDIR)/$(SOBINRELDIR)/$(GS_SONAME)
	$(RM_) $(BINDIR)/$(SOBINRELDIR)/$(GS_SONAME_MAJOR)
	$(RM_) $(GSSOC)
	$(RM_) $(GSSOX)

# End of unix-dll.mak
