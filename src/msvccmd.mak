#    Copyright (C) 1997, 2000 artofcode LLC.  All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA, 02111-1307.


# $Id: msvccmd.mak,v 1.1 2004/01/14 16:59:52 atai Exp $
# Command definition section for Microsoft Visual C++ 4.x/5.x,
# Windows NT or Windows 95 platform.
# Created 1997-05-22 by L. Peter Deutsch from msvc4/5 makefiles.
# edited 1997-06-xx by JD to factor out interpreter-specific sections
# edited 2000-03-30 by lpd to make /FPi87 conditional on MSVC version
# edited 2000-06-05 by lpd to treat empty MSINCDIR and LIBDIR specially.

# Set up linker differently for MSVC 4 vs. later versions

!if $(MSVC_VERSION) == 4

# MSVC version 4.x doesn't recognize the /QI0f switch, which works around
# an unspecified bug in the Pentium decoding of certain 0F instructions.
QI0f=

!else
#else $(MSVC_VERSION) != 4

# MSVC 5.x does recognize /QI0f.
QI0f=/QI0f

# Define separate CCAUX command-line switch that must be at END of line.

CCAUX_TAIL= /link

!endif
#endif #$(MSVC_VERSION) == 4


# Define the current directory prefix and shell invocations.

D=\#

SH=

# Define switches for the compilers.

C_=
O_=-Fo
RO_=$(O_)

# Define the arguments for genconf.

CONFILES=-p %%s -l $(GLGENDIR)\lib.tr
CONFLDTR=-o

# Define the generic compilation flags.

PLATOPT=

INTASM=
PCFBASM=

# Make sure we get the right default target for make.

dosdefault: default

# Define the compilation flags.

!if "$(CPU_FAMILY)"=="i386"

!if $(CPU_TYPE)>500
CPFLAGS=/G5 $(QI0f)
!else if $(CPU_TYPE)>400
CPFLAGS=/GB $(QI0f)
!else
CPFLAGS=/GB $(QI0f)
!endif

!if $(FPU_TYPE)>0 && $(MSVC_VERSION)<5
FPFLAGS=/FPi87
!else
FPFLAGS=
!endif

!endif

!if "$(CPU_FAMILY)"=="ppc"

!if $(CPU_TYPE)>=620
CPFLAGS=/QP620
!else if $(CPU_TYPE)>=604
CPFLAGS=/QP604
!else
CPFLAGS=/QP601
!endif

FPFLAGS=

!endif

!if "$(CPU_FAMILY)"=="alpha"

# *** alpha *** This needs fixing
CPFLAGS=
FPFLAGS=

!endif

!if $(NOPRIVATE)!=0
CP=/DNOPRIVATE
!else
CP=
!endif

!if $(DEBUG)!=0
CD=/DDEBUG
!else
CD=
!endif

!if $(TDEBUG)!=0
# /Fd designates the directory for the .pdb file.
# Note that it must be followed by a space.
CT=/Zi /Od /Fd$(GLOBJDIR) $(NULL)
LCT=/DEBUG
COMPILE_FULL_OPTIMIZED=    # no optimization when debugging
COMPILE_WITH_FRAMES=    # no optimization when debugging
COMPILE_WITHOUT_FRAMES=    # no optimization when debugging
!else
CT=
LCT=
# NOTE: With MSVC++ 5.0, /O2 produces a non-working executable.
# We believe the following list of optimizations works around this bug.
COMPILE_FULL_OPTIMIZED=/GF /Ot /Oi /Ob2 /Oy /Oa- /Ow-
COMPILE_WITH_FRAMES=
COMPILE_WITHOUT_FRAMES=/Oy
!endif

!if $(DEBUG)!=0 || $(TDEBUG)!=0
CS=/Ge
!else
CS=/Gs
!endif

# Specify output object name
CCOBJNAME=-Fo

# Specify function prolog type
COMPILE_FOR_DLL=
COMPILE_FOR_EXE=
COMPILE_FOR_CONSOLE_EXE=


# The /MT is for multi-threading.  We would like to make this an option,
# but it's too much work right now.
GENOPT=$(CP) $(CD) $(CT) $(CS) /W2 /nologo /MT

CCFLAGS=$(PLATOPT) $(FPFLAGS) $(CPFLAGS) $(CFLAGS) $(XCFLAGS)
CC=$(COMP) /c $(CCFLAGS) @$(GLGENDIR)\ccf32.tr
CPP=$(COMPCPP) /c $(CCFLAGS) @$(GLGENDIR)\ccf32.tr
!if $(MAKEDLL)
WX=$(COMPILE_FOR_DLL)
!else
WX=$(COMPILE_FOR_EXE)
!endif
# /Za disables the MS-specific extensions & enables ANSI mode.
CC_WX=$(CC) $(WX)
CC_=$(CC_WX) $(COMPILE_FULL_OPTIMIZED) /Za
CC_D=$(CC_WX) $(COMPILE_WITH_FRAMES)
CC_INT=$(CC_)
CC_LEAF=$(CC_) $(COMPILE_WITHOUT_FRAMES)
CC_NO_WARN=$(CC_)

# Compiler for auxiliary programs

CCAUX=$(COMPAUX) /O

# Compiler for Windows programs.
# /Ze enables MS-specific extensions (this is also the default).

CCWINFLAGS=$(COMPILE_FULL_OPTIMIZED) /Ze

#end msvccmd.mak
