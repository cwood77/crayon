@echo off

rem unit
bin\out\debug\crayon test > nul
if not errorlevel 0 goto fail

rem basic
del testdata\out-*.bmp > nul 2>&1
bin\out\debug\crayon testdata\basic.cray > nul
if not errorlevel 0 goto fail
fc /b testdata\scribble.bmp testdata\out-scribble.bmp > nul
if not errorlevel 0 goto fail

rem snip/overlay
bin\out\debug\crayon testdata\snip+overlay.cray > nul
if not errorlevel 0 goto fail
fc /b testdata\expected-scribbleWithOverlay.bmp testdata\out-scribbleWithOverlay.bmp > nul
if not errorlevel 0 goto fail

:win
echo all clear
goto end

:fail
echo TEST FAILED

:end
