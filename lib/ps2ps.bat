@echo off 
@rem $Id: ps2ps.bat,v 1.2 2004/02/14 22:20:04 atai Exp $
@rem "Distill" PostScript.

if %1/==/ goto usage
if %2/==/ goto usage
call gssetgs.bat
echo -dNODISPLAY -dNOPAUSE -dSAFER -dBATCH >_.at
:cp
if %3/==/ goto doit
echo %1 >>_.at
shift
goto cp

:doit
rem Watcom C deletes = signs, so use # instead.
%GSC% -q -sDEVICE#pswrite -sOutputFile#%2 @_.at %1
goto end

:usage
echo "Usage: ps2ps [options] input.ps output.ps"
echo "  e.g. ps2ps -sPAPERSIZE=a4 input.ps output.ps

:end