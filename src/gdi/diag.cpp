#include "api.hpp"

static int CALLBACK diagCallback(
   const LOGFONTA *lpelfe,
   const TEXTMETRICA *lpntme,
   DWORD FontType,
   LPARAM lParam)
{
   log& Log = *(log*)lParam;

   Log.s().s() << lpelfe->lfFaceName << " {" << std::endl;
   {
      autoIndent _a(Log);

      Log.s().s() << "FontType=" << FontType << std::endl;
#define cdwDiagField(__field__) "LOGFONT::" << #__field__ << " = " << (LONG)lpelfe->__field__
      Log.s().s() << cdwDiagField(lfHeight) << std::endl;
      Log.s().s() << cdwDiagField(lfWidth) << std::endl;
      Log.s().s() << cdwDiagField(lfEscapement) << std::endl;
      Log.s().s() << cdwDiagField(lfOrientation) << std::endl;
      Log.s().s() << cdwDiagField(lfWeight) << std::endl;
      Log.s().s() << cdwDiagField(lfItalic) << std::endl;
      Log.s().s() << cdwDiagField(lfUnderline) << std::endl;
      Log.s().s() << cdwDiagField(lfStrikeOut) << std::endl;
      Log.s().s() << cdwDiagField(lfCharSet) << std::endl;
      Log.s().s() << cdwDiagField(lfOutPrecision) << std::endl;
      Log.s().s() << cdwDiagField(lfClipPrecision) << std::endl;
      Log.s().s() << cdwDiagField(lfQuality) << std::endl;
      Log.s().s() << cdwDiagField(lfPitchAndFamily) << std::endl;
#undef cdwDiagField
#define cdwDiagField(__field__) "TEXTMETRICA::" << #__field__ << " = " << (LONG)lpntme->__field__
      Log.s().s() << cdwDiagField(tmHeight) << std::endl;
      Log.s().s() << cdwDiagField(tmAscent) << std::endl;
      Log.s().s() << cdwDiagField(tmDescent) << std::endl;
      Log.s().s() << cdwDiagField(tmInternalLeading) << std::endl;
      Log.s().s() << cdwDiagField(tmExternalLeading) << std::endl;
      Log.s().s() << cdwDiagField(tmAveCharWidth) << std::endl;
      Log.s().s() << cdwDiagField(tmMaxCharWidth) << std::endl;
      Log.s().s() << cdwDiagField(tmWeight) << std::endl;
      Log.s().s() << cdwDiagField(tmOverhang) << std::endl;
      Log.s().s() << cdwDiagField(tmDigitizedAspectX) << std::endl;
      Log.s().s() << cdwDiagField(tmDigitizedAspectY) << std::endl;
      Log.s().s() << cdwDiagField(tmFirstChar) << std::endl;
      Log.s().s() << cdwDiagField(tmLastChar) << std::endl;
      Log.s().s() << cdwDiagField(tmDefaultChar) << std::endl;
      Log.s().s() << cdwDiagField(tmBreakChar) << std::endl;
      Log.s().s() << cdwDiagField(tmItalic) << std::endl;
      Log.s().s() << cdwDiagField(tmUnderlined) << std::endl;
      Log.s().s() << cdwDiagField(tmStruckOut) << std::endl;
      Log.s().s() << cdwDiagField(tmPitchAndFamily) << std::endl;
      Log.s().s() << cdwDiagField(tmCharSet) << std::endl;
#undef cdwDiagField
   }
   Log.s().s() << "}" << std::endl;

   return 1; // continue
}

void api::diagnostics()
{
   Log.s().s() << "DPI = " << ::GetDpiForSystem() << std::endl;

   auto dc = ::CreateCompatibleDC(NULL);

   auto lpY = ::GetDeviceCaps(dc,LOGPIXELSY);
   Log.s().s() << "logpixY = " << lpY << std::endl;

   char buffer[MAX_PATH];
   ::GetTextFaceA(dc,MAX_PATH,buffer);
   Log.s().s() << "default GDI font face = " << buffer << std::endl;
   TEXTMETRICA tm;
   ::GetTextMetrics(dc,&tm);
   Log.s().s() << "default GDI font size = " << (tm.tmHeight * 72 / lpY)
      << " (" << tm.tmHeight << ")" << std::endl;

   LOGFONTA lFont;
   lFont.lfCharSet = DEFAULT_CHARSET; // all charsets
   lFont.lfFaceName[0] = 0; // find one of each face
   lFont.lfPitchAndFamily = 0; // reserved

   Log.s().s() << "dumping all system fonts = {" << std::endl;
   {
      autoIndent _a(Log);
      ::EnumFontFamiliesExA(
         dc,            // [in] HDC           hdc,
         &lFont,        // [in] LPLOGFONTA    lpLogfont,
         &diagCallback, // [in] FONTENUMPROCA lpProc,
         (LPARAM)&Log,  // [in] LPARAM        lParam,
         0              // DWORD         dwFlags
      );
   }
   Log.s().s() << "}" << std::endl;

   lFont.lfCharSet = ANSI_CHARSET;
   ::strcpy(lFont.lfFaceName,"Calibri");
   lFont.lfPitchAndFamily = 0;

   Log.s().s() << "dumping all ANSI calibri fonts = {" << std::endl;
   {
      autoIndent _a(Log);
      ::EnumFontFamiliesExA(
         dc,            // [in] HDC           hdc,
         &lFont,        // [in] LPLOGFONTA    lpLogfont,
         &diagCallback, // [in] FONTENUMPROCA lpProc,
         (LPARAM)&Log,  // [in] LPARAM        lParam,
         0              // DWORD         dwFlags
      );
   }
   Log.s().s() << "}" << std::endl;

   ::DeleteDC(dc);
}
