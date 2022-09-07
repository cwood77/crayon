@echo off

rem unit
bin\out\debug\crayon test > nul
if not errorlevel 0 goto fail

rem basic
del testdata\out-*.bmp > nul 2>&1
bin\out\debug\crayon testdata\basic.cray > nul
if not errorlevel 0 goto fail
fc /b testdata\scribble.bmp testdata\out-scribble.bmp
if not errorlevel 0 goto fail

rem snip, overlay and frame
bin\out\debug\crayon testdata\snip+overlay.cray > nul
if not errorlevel 0 goto fail
fc /b testdata\expected-scribbleWithOverlay.bmp testdata\out-scribbleWithOverlay.bmp
if not errorlevel 0 goto fail
fc /b testdata\expected-scribbleWithOverlayDeFramed.bmp testdata\out-scribbleWithOverlayDeFramed.bmp
if not errorlevel 0 goto fail

rem find object
bin\out\debug\crayon testdata\find-object.cray > nul
if not errorlevel 0 goto fail
fc /b testdata\expected-found.bmp testdata\out-found.bmp
if not errorlevel 0 goto fail
fc /b testdata\expected-found3.bmp testdata\out-found3.bmp
if not errorlevel 0 goto fail

rem tick rects
bin\out\debug\crayon testdata\place-glyph.cray > nul
if not errorlevel 0 goto fail
   rem calc-tick-rect / rgb{255,0,0} -> r
   rem overlay $g rgb(255,255,255) $r

:win
echo all clear
goto end

:fail
echo TEST FAILED

:end
