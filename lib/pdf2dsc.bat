@echo off 
@rem $Id: pdf2dsc.bat,v 1.1 2004/01/14 16:59:47 atai Exp $
@rem Convert PDF to DSC

if %1/==/ goto usage
if %2/==/ goto usage
call gssetgs.bat

rem Watcom C deletes = signs, so use # instead.
%GSC% -q -dNODISPLAY -dSAFER -dDELAYSAFER -sPDFname#%1 -sDSCname#%2 -sOutputFile#%2 pdf2dsc.ps
goto end

:usage
echo "Usage: pdf2dsc input.pdf output.dsc"

:end


