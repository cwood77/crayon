@echo off

rem unit
bin\out\debug\crayon test > nul
if not errorlevel 0 goto fail

del testdata\out-*.bmp > nul 2>&1

rem basic
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
fc /b testdata\expected-cardWithGlyph.bmp testdata\out-cardWithGlyph.bmp
if not errorlevel 0 goto fail

rem draw text (point)
bin\out\debug\crayon testdata\drawtext.cray > nul
if not errorlevel 0 goto fail
fc /b testdata\expected-text.bmp testdata\out-text.bmp
if not errorlevel 0 goto fail

rem draw text (rect) + string reading
bin\out\debug\crayon testdata\hamlet.cray > nul
if not errorlevel 0 goto fail
fc /b testdata\expected-recttext-garden.bmp testdata\out-recttext-garden.bmp
if not errorlevel 0 goto fail
fc /b testdata\expected-recttext-yorick.bmp testdata\out-recttext-yorick.bmp
if not errorlevel 0 goto fail

:win
echo all clear
goto end

:fail
echo TEST FAILED

:end
