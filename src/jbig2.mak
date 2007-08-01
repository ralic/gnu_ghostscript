#  Copyright (C) 2003-2006 artofcode LLC.
#  All Rights Reserved.
#
#  This file is part of GNU ghostscript
#
#  GNU ghostscript is free software; you can redistribute it and/or
#  modify it under the terms of the version 2 of the GNU General
#  Public License as published by the Free Software Foundation.
#
#  GNU ghostscript is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along with
#  ghostscript; see the file COPYING. If not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# $Id: jbig2.mak,v 1.8 2007/08/01 14:26:39 jemarch Exp $

# makefile for jbig2dec library code.
# Users of this makefile must define the following:
#	SHARE_JBIG2 - whether to compile in or link to the library
#	JBIG2SRCDIR - the library source directory
#
# gs.mak and friends define the following:
#	JBIG2OBJDIR - the output obj directory
#	JBIG2GENDIR - generated (.dev) file directory
#	JB2I_ JB2CF_ - include and cflags for compiling the lib

# We define the jbig2dec.dev target and its dependencies
#
# This partial makefile compiles the jbig2dec library for use in
# Ghostscript.

# Define the name of this makefile.
JBIG2_MAK=$(GLSRC)jbig2.mak

JBIG2SRC=$(JBIG2SRCDIR)$(D)
JBIG2GEN=$(JBIG2OBJDIR)$(D)
JBIG2OBJ=$(JBIG2OBJDIR)$(D)

# This makefile is only known to work with jbig2dec v0.7 and later
# to use an earlier version, remove unknown files from
# the OBJS lists below

libjbig2_OBJS1=\
	$(JBIG2OBJ)jbig2.$(OBJ) \
	$(JBIG2OBJ)jbig2_arith.$(OBJ) \
        $(JBIG2OBJ)jbig2_arith_iaid.$(OBJ) \
        $(JBIG2OBJ)jbig2_arith_int.$(OBJ) \
        $(JBIG2OBJ)jbig2_generic.$(OBJ) \
        $(JBIG2OBJ)jbig2_refinement.$(OBJ) \
        $(JBIG2OBJ)jbig2_huffman.$(OBJ) \
        $(JBIG2OBJ)jbig2_image.$(OBJ) \
        $(JBIG2OBJ)jbig2_mmr.$(OBJ)

libjbig2_OBJS2=\
	$(JBIG2OBJ)jbig2_page.$(OBJ) \
        $(JBIG2OBJ)jbig2_segment.$(OBJ) \
        $(JBIG2OBJ)jbig2_symbol_dict.$(OBJ) \
        $(JBIG2OBJ)jbig2_text.$(OBJ) \
        $(JBIG2OBJ)jbig2_metadata.$(OBJ) $(JBIG2_EXTRA_OBJS)

libjbig2_OBJS=$(libjbig2_OBJS1) $(libjbig2_OBJS2)

libjbig2_HDRS=\
        $(JBIG2SRC)jbig2.h \
        $(JBIG2SRC)jbig2_arith.h \
        $(JBIG2SRC)jbig2_arith_iaid.h \
        $(JBIG2SRC)jbig2_arith_int.h \
        $(JBIG2SRC)jbig2_generic.h \
        $(JBIG2SRC)jbig2_huffman.h \
        $(JBIG2SRC)jbig2_hufftab.h \
        $(JBIG2SRC)jbig2_image.h \
        $(JBIG2SRC)jbig2_mmr.h \
        $(JBIG2SRC)jbig2_priv.h \
        $(JBIG2SRC)jbig2_symbol_dict.h \
        $(JBIG2SRC)jbig2_metadata.h \
        $(JBIG2SRC)config_win32.h

jbig2dec_OBJS=$(JBIG2OBJ)getopt.$(OBJ) $(JBIG2OBJ)getopt1.$(OBJ) $(JBIG2OBJ)sha1.$(OBJ)
jbig2dec_HDRS=$(JBIG2OBJ)getopt.h $(JBIG2OBJ)sha1.h

jbig2.clean : jbig2.config-clean jbig2.clean-not-config-clean

### WRONG.  MUST DELETE OBJ AND GEN FILES SELECTIVELY.
jbig2.clean-not-config-clean :
	$(EXP)$(ECHOGS_XE) $(JBIG2SRCDIR) $(JBIG2OBJDIR)
	$(RM_) $(JBIG2OBJDIR)$(D)*..$(OBJ)

jbig2.config-clean :
	$(RMN_) $(JBIG2GEN)$(D)jbig2*.dev

JBIG2DEP=$(AK)

JBIG2_CC=$(CC_) $(CFLAGS) $(I_)$(JBIG2GENDIR) $(II)$(JB2I_)$(_I) $(JB2CF_)
JBIG2O_=$(O_)$(JBIG2OBJ)

# switch in the version of libjbig2.dev we're actually using
$(JBIG2GEN)jbig2dec.dev : $(TOP_MAKEFILES) $(JBIG2GEN)jbig2dec_$(SHARE_JBIG2).dev
	$(CP_) $(JBIG2GEN)jbig2dec_$(SHARE_JBIG2).dev $(JBIG2GEN)jbig2dec.dev

# dev file for shared (separately built) jbig2dec library
$(JBIG2GEN)jbig2dec_1.dev : $(TOP_MAKEFILES) $(JBIG2_MAK) $(ECHOGS_XE)
	$(SETMOD) $(JBIG2GEN)jbig2dec_1 -lib jbig2dec

# dev file for compiling our own from source
$(JBIG2GEN)jbig2dec_0.dev : $(TOP_MAKEFILES) $(JBIG2_MAK) $(ECHOGS_XE) $(libjbig2_OBJS)
	$(SETMOD) $(JBIG2GEN)jbig2dec_0 $(libjbig2_OBJS1)
	$(ADDMOD) $(JBIG2GEN)jbig2dec_0 $(libjbig2_OBJS2)

# explicit rules for building the source files. 

$(JBIG2OBJ)snprintf.$(OBJ) : $(JBIG2SRC)snprintf.c $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)snprintf.$(OBJ) $(C_) $(JBIG2SRC)snprintf.c

$(JBIG2OBJ)getopt.$(OBJ) : $(JBIG2SRC)getopt.c $(JBIG2SRC)getopt.h $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)getopt.$(OBJ) $(C_) $(JBIG2SRC)getopt.c

$(JBIG2OBJ)getopt1.$(OBJ) : $(JBIG2SRC)getopt1.c $(JBIG2SRC)getopt.h $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)getopt1.$(OBJ) $(C_) $(JBIG2SRC)getopt1.c

$(JBIG2OBJ)jbig2.$(OBJ) : $(JBIG2SRC)jbig2.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2.$(OBJ) $(C_) $(JBIG2SRC)jbig2.c

$(JBIG2OBJ)jbig2_arith.$(OBJ) : $(JBIG2SRC)jbig2_arith.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_arith.$(OBJ) $(C_) $(JBIG2SRC)jbig2_arith.c

$(JBIG2OBJ)jbig2_arith_iaid.$(OBJ) : $(JBIG2SRC)jbig2_arith_iaid.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_arith_iaid.$(OBJ) $(C_) $(JBIG2SRC)jbig2_arith_iaid.c

$(JBIG2OBJ)jbig2_arith_int.$(OBJ) : $(JBIG2SRC)jbig2_arith_int.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_arith_int.$(OBJ) $(C_) $(JBIG2SRC)jbig2_arith_int.c

$(JBIG2OBJ)jbig2_generic.$(OBJ) : $(JBIG2SRC)jbig2_generic.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_generic.$(OBJ) $(C_) $(JBIG2SRC)jbig2_generic.c

$(JBIG2OBJ)jbig2_refinement.$(OBJ) : $(JBIG2SRC)jbig2_refinement.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_refinement.$(OBJ) $(C_) $(JBIG2SRC)jbig2_refinement.c
 
$(JBIG2OBJ)jbig2_huffman.$(OBJ) : $(JBIG2SRC)jbig2_huffman.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_huffman.$(OBJ) $(C_) $(JBIG2SRC)jbig2_huffman.c

$(JBIG2OBJ)jbig2_image.$(OBJ) : $(JBIG2SRC)jbig2_image.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_image.$(OBJ) $(C_) $(JBIG2SRC)jbig2_image.c

$(JBIG2OBJ)jbig2_image_pbm.$(OBJ) : $(JBIG2SRC)jbig2_image_pbm.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_image_pbm.$(OBJ) $(C_) $(JBIG2SRC)jbig2_image_pbm.c

$(JBIG2OBJ)jbig2_image_png.$(OBJ) : $(JBIG2SRC)jbig2_image_png.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_image_png.$(OBJ) $(C_) $(JBIG2SRC)jbig2_image_png.c

$(JBIG2OBJ)jbig2_mmr.$(OBJ) : $(JBIG2SRC)jbig2_mmr.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_mmr.$(OBJ) $(C_) $(JBIG2SRC)jbig2_mmr.c

$(JBIG2OBJ)jbig2_page.$(OBJ) : $(JBIG2SRC)jbig2_page.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_page.$(OBJ) $(C_) $(JBIG2SRC)jbig2_page.c

$(JBIG2OBJ)jbig2_segment.$(OBJ) : $(JBIG2SRC)jbig2_segment.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_segment.$(OBJ) $(C_) $(JBIG2SRC)jbig2_segment.c

$(JBIG2OBJ)jbig2_symbol_dict.$(OBJ) : $(JBIG2SRC)jbig2_symbol_dict.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_symbol_dict.$(OBJ) $(C_) $(JBIG2SRC)jbig2_symbol_dict.c

$(JBIG2OBJ)jbig2_text.$(OBJ) : $(JBIG2SRC)jbig2_text.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_text.$(OBJ) $(C_) $(JBIG2SRC)jbig2_text.c

$(JBIG2OBJ)jbig2_metadata.$(OBJ) : $(JBIG2SRC)jbig2_metadata.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2_metadata.$(OBJ) $(C_) $(JBIG2SRC)jbig2_metadata.c

$(JBIG2OBJ)jbig2dec.$(OBJ) : $(JBIG2SRC)jbig2dec.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)jbig2dec.$(OBJ) $(C_) $(JBIG2SRC)jbig2dec.c

$(JBIG2OBJ)sha1.$(OBJ) : $(JBIG2SRC)sha1.c $(libjbig2_HDRS) $(JBIG2DEP)
	$(JBIG2_CC) $(JBIG2O_)sha1.$(OBJ) $(C_) $(JBIG2SRC)sha1.c

