#!/usr/bin/tclsh

#    Copyright (C) 2000-2002 artofcode LLC. All rights reserved.
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

# $Id: pre.tcl,v 1.1 2004/02/14 22:32:19 atai Exp $

# Check various aspects of an about-to-be-released Ghostscript fileset.
# Only applicable to filesets 6.0 and later (assumes use of CVS).
# Note: requires the GNU 'date' -r switch and - format character.

set DIFFFILE t
set GREPFILE t1
set CHECKFILE /gs/toolbin/pre.chk
set GSINIT lib/gs_init.ps
set NEWS doc/News.htm

# ---------------- Utilities ---------------- #

# Print an error message.
proc message {str} {
    puts stderr "**** $str"
}

# Search for one or more occurrences of a pattern in a file.
# If any are found, set $linevar to the line containing the first occurrence,
# and return 1; otherwise, return 0.
proc grep1 {pattern file linevar} {
    upvar $linevar line
    set line ""
    set cmd [list exec egrep $pattern $file]
    catch "set line \[$cmd\]"
    if {$line == ""} {return 0}
    set line [lindex [split $line "\n"] 0]
    return 1
}

# Spell out the name of the month, given then number. Month numbers with
# leading zeros are accepted.
proc monthname {monthnum} {
    # Define a 1-origin array of month names.
    set monthnames1 [list ?\
	    January February March April May June\
	    July August September October November December]
    regexp {^0*([0-9]+)} $monthnum skip index
    return [lindex $monthnames1 $index]
}

# ---------------- Main program ---------------- #

################ Check the arguments

if {$argv != {} && $argv != {update}} {
    puts stderr "Usage: pre [update]"
    exit 1
}

################ Get the date and version number from doc/News.htm

proc read_news {} {
    global NEWS NEWSVERSION rdate ryear rmonth rday vcurdot

    if {![grep1 {^Version [0-9.]+[( ]} $NEWS vdate]} {
	if {![grep1 {[<]/a>Version [0-9.]+[( ]} $NEWS vdate]} {
	    message "Unable to parse $NEWS!"
	    exit 1
	}
    }
    regsub -all {<[^>]*>} $vdate "" vdate
    puts "Most recent version in $NEWS: $vdate"
    set vymdpat {^Version [0-9.]+(.*)\((([0-9x]+)-([0-9x]+)-([0-9x]+))\)$}
    if {![regexp $vymdpat $vdate skip skip2 rdate ryear rmonth rday]} {
	message "$NEWS release date is missing"
	exit 1
    }
    set curdate [exec date -u +%Y-%m-%d]
    if {$rdate != $curdate} {
	message "Warning: current date is $curdate, release date is $rdate"
    }
    set NEWSVERSION $vdate
}

read_news

################ Check the date and version number in version.mak

proc check_version {mak} {
    global rdate ryear rmonth rday vcurdot
# Check the date.
    if ![grep1 {GS_REVISIONDATE[ ]*=} $mak ddate] {
	message "Unable to find date line in $mak"
    } elseif {![regexp {([0-9]+)} $ddate skip ddate]} {
	message "Unable to parse date in $mak"
    } elseif {![string match *x* $rday]} {
	if {$ddate != "${ryear}${rmonth}${rday}"} {
	    message "Date $rdate in NEWS doesn't match date $ddate in $mak"
	}
    }
# Check the version number.
    if ![grep1 {GS_VERSION_MAJOR[ ]*=} $mak major] {
	message "Can't find GS_VERSION_MAJOR in $mak"
    } elseif ![grep1 {GS_VERSION_MINOR[ ]*=} $mak minor] {
	message "Can't find GS_VERSION_MINOR in $mak"
    } else {
	regexp {=[ "]*([0-9]+)[ "]*$} $major skip vmajor
	regexp {=[ "]*([0-9]+)[ "]*$} $minor skip vminor
	if {"${vmajor}.${vminor}" != $vcurdot} {
	    message "In $mak: DOT_VERSION=${vmajor}.${vminor}, current is $vcurdot"
	}
    }
}

################ Get the version number from gs_init.ps

if ![grep1 {^[0-9]+($|[ 	]*%)} $GSINIT version] {
    message "No version number found in $GSINIT!"
    exit
}
regexp {^[0-9]+} $version version
puts "Current version in $GSINIT is $version"

################ Check various version #s and related information

regexp {^Version ([0-9.]+)} $NEWSVERSION skip vcurdot
regsub -all {\.} $vcurdot {} vcur
if [regexp {^[0-9]\.[0-9]$} $vcurdot] {
    append vcur 0
}
if {$vcur != $version} {
    message "$vcur != $version, exiting"
    exit
}

################ Check the third-party library version #s in the makefiles

proc check_library_version {defname version} {
    set matches [eval exec grep ${defname}= [glob src/*.mak]]
    foreach match [split $matches "\n"] {
	if {![regexp "^(.*):\[ 	\]*${defname}=(\[0-9\]+)(\[^0-9\]|\$)" $match skip file defver]} {
	    message "Unrecognized result from grep: $match"
	} elseif {$defver != $version} {
	    message "$file has ${defname}=$defver, current is $version"
	}
    }
}

set VFILE jpeg/jversion.h
if ![grep1 {^#define JVERSION[ 	]+"([0-9]+)[^0-9]} $VFILE line] {
    message "No version number found in $VFILE!"
    exit
}
regexp {^#define JVERSION[ 	]+"([0-9]+)[^0-9]} $line skip jversion
puts "Current jpeg version is $jversion"
set VFILE libpng/png.h
if ![grep1 {^#define PNG_LIBPNG_VER[ 	]+([0-9]+)[^0-9]} $VFILE line] {
    message "No version number found in $VFILE!"
    exit
}
regexp {^#define PNG_LIBPNG_VER[ 	]+([0-9]+)[^0-9]} $line skip pversion
puts "Current libpng version is $pversion"
check_library_version JVERSION $jversion
check_library_version PVERSION $pversion

################ If requested, mechanically update doc files.

set dateformat "%-d %B %Y"
set curdate [exec date -u +$dateformat]
set manlist [glob {man/gs*.1} {man/[dpfw]*.1}]
foreach d "[glob doc/*.htm] doc/gs-vms.hlp" {
    if {![regexp {Changes|Hershey|Humor|Public} $d]} {
	lappend doclist $d
    }
}

if {$argv == {update}} {
	# Update dates in .htm and .1 files.
    proc updoc {doc before after} {
	set tmpfile /tmp/[pid]
	catch {file delete $tmpfile}
	exec perl -pwe "s{$before}{$after}" < $doc > $tmpfile
	file rename -force $tmpfile $doc
    }
    set rever {[0-9.]+}
    set redate {[0-9]+ [A-Z][a-z]+ [0-9]+}
    foreach doc $doclist {
	updoc $doc \
		"^(! |<small>)(AFPL |Aladdin |GNU |)(Ghostscript version )$rever, ${redate}(</small>|)\$" \
	    "\$1\$2\${3}$vcurdot, $curdate"
    }
    foreach doc1 $manlist {
	updoc $doc1 \
	    "^(\\.TH \[A-Z0-9\]+ 1 )\"$redate\" $rever " \
	    "\${1}\"$curdate\" $vcurdot "
	updoc $doc1 \
	    "^(This document was last revised for Ghostscript version )$rever\\.\$" \
	    "\${1}${vcurdot}."
    }
    exit
}

file delete $DIFFFILE $GREPFILE
exec touch $DIFFFILE

################ Check dates and version #s in makefiles

check_version src/version.mak

################ Check dates and version #s in documentation files

foreach doc $doclist {
    set dateline {^(! |<small>)(AFPL |Aladdin |GNU |)Ghostscript version ([0-9.]+), ([0-9]+ [A-Z][a-z]+ [0-9]+)(</small>|)$}
    set idline {[$]Id: [^,]+,v ([^ ]+) ([0-9][0-9][0-9][0-9])/([0-9][0-9])/([0-9][0-9]) }
    if {![grep1 $dateline $doc dstr]} {
	message "Can't find version # and date in $doc"
    } elseif {![grep1 $idline $doc idstr]} {
	message "Can't find Id line in $doc"
    } else {
	regexp $dateline $dstr skip skip2 skip3 dver ddate
	regexp $idline $idstr skip idver year month1 day
	set ddate [exec date -u +$dateformat -d $ddate]
	# Strip leading 0 from month and day.
	set day [expr 1$day - 100]
	set month1 [expr 1$month1 - 100]
	set idate "$day [monthname $month1] $year"
	if {$ddate != $idate} {
	    # Make a special exception for CVS version 1.1: see below.
	    if {$idver != "1.1"} {
		message "$doc shows version $dver, $ddate;\n\tcurrent is $vcurdot, last updated $idate"
	    }
	}
    }
}

################ Check dates and version #s in man pages

foreach doc $manlist {
    set in [open $doc]
    set idline [gets $in]
    set thline [gets $in]
    close $in
    # We must be careful not to include the string $,I,d,: in any pattern,
    # since CVS/RCS will substitute for it!
    if {![regexp {^\.\\" [$]Id: ([^,]+),v ([0-9.]+) ([0-9][0-9][0-9][0-9])/([0-9][0-9])/([0-9][0-9])} $idline skip file idrevision idyear idmonth idday]} {
	message "In $doc, can't parse $Id line: $idline"
	continue
    }
    if {$file != [file tail $doc]} {
	message "In $doc, File name $file in Id: line doesn't match."
	continue
    }
    set prefix ".TH [string toupper [file root [file tail $doc]]] 1 \""
    if {[string first $prefix $thline] != 0} {
	message "In $doc, .TH line doesn't match: $thline"
	continue
    }
    if {![regexp {"([0-9]+) ([A-Z][a-z]+) ([0-9]+)"} $thline skip thday thmonth thyear]} {
	message "In $doc, can't parse date in .TH line: $thline"
	continue
    }
    if {![regexp {" ([0-9.]+) ("Ghostscript Tools"|Ghostscript) } $thline skip dver]} {
	message "In $doc, can't find version # and 'Ghostscript' in .TH line: $thline"
	continue
    }
    # The first requirement we enforce is that the change date in
    # the Id: line match the one in the .TH line.  If they don't
    # match, we also check the version # at the top and bottom of
    # the file.  We make a special exception if the revision # is 1.1
    # and the Ghostscript version # is 6.0, because the very first
    # checkin date for the repository was later than the release date.
    if {$idrevision != 1.1 || $dver != 6.0} {
	if {$idyear != $thyear || [monthname $idmonth] != $thmonth || $idday != $thday} {
	    message "In $doc, Id: date is $idyear/$idmonth/$idday, .TH date is $thday $thmonth $thyear"
	    if {$dver != $vcurdot} {
		message "$doc header cites version $dver, current is $vcurdot"
	    }
	    if {![grep1 {^This document was last revised for Ghostscript version [0-9.]+\.$} $doc rstr]} {
		message "$doc doesn't contain a last-revision line."
	    } else {
		regexp {([0-9.]+)\.$} $rstr skip dver
		if {$dver != $vcurdot} {
		    message "In $doc, last revision is $dver, current is $vcurdot"
		}
	    }
	}
    }
}

################ Check miscellaneous parameters in makefiles and code

proc grept1 {pattern files {force_filename 1}} {
    global DIFFFILE GREPFILE
	# We include t in the lists below to force grep to include the file
	# name in the output, because there is no switch to do this.
    if $force_filename {
	set t $DIFFFILE
    } else {
	set t ""
    }
    catch {eval exec grep $pattern $files $t >> $GREPFILE}
}
set unix_maks {src/unixansi.mak src/unixtrad.mak src/unix-gcc.mak src/dvx-gcc.mak}
set pc_maks {src/bcwin32.mak src/msvc32.mak src/watc.mak src/watcwin.mak}
set lib_maks {src/msvclib.mak src/watclib.mak src/ugcclib.mak}
set all_maks [concat $unix_maks $pc_maks $lib_maks]
grept1 ^GENOPT= $unix_maks
grept1 ^CFLAGS= $unix_maks
grept1 ^XCFLAGS= $unix_maks
grept1 ^LDFLAGS= $unix_maks
grept1 ^XLIBS= $unix_maks
grept1 ^ZLIB_NAME= $unix_maks
grept1 ^DEBUG= $pc_maks
grept1 ^TDEBUG= $pc_maks
grept1 ^NOPRIVATE= $pc_maks
grept1 bgi_= src/devs.mak
grept1 mswin_= src/devs.mak
grept1 ^BAND_LIST_ $all_maks
grept1 ^COMPILE_INITS= $unix_maks
grept1 DEVICE_DEVS $all_maks
grept1 FEATURE_DEVS $all_maks
grept1 ^SHARE_ $unix_maks
grept1 ^..._TYPE= $pc_maks
grept1 define._fixed_shift src/gxfixed.h
grept1 define.TRIM src/gdevbit.c
grept1 define.EXPAND src/gdevbit.c
grept1 if...1...return.0 src/gdevxalt.c
grept1 define.TEST [glob src/gdev*.c]
grept1 define.PDFX src/gdevpdfx.h
grept1 define.FORCE src/gshtscr.c
grept1 define.CAPTURE src/gslib.c
grept1 interval.= src/zcontext.c
# Apparently diff exits with a non-zero status if there are any differences!
catch {exec diff $GREPFILE $CHECKFILE >> $DIFFFILE}
exec wc $DIFFFILE >@ stdout