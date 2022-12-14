#include "../crayon/path.hpp"
#include "api.hpp"

api::api(iLog& l)
: Log(l,"[gdi+] "), m_gdiPlusToken(0)
{
   using namespace Gdiplus;
   Log.s().s() << "creating API" << std::endl;
   GdiplusStartupInput in;
   auto success = GdiplusStartup(
      &m_gdiPlusToken,
      &in,
      NULL);
   if(success != Ok)
      throw std::runtime_error("failed to initialize GDI+");
}

api::~api()
{
   using namespace Gdiplus;
   Log.s().s() << "closing API" << std::endl;
   GdiplusShutdown(m_gdiPlusToken);
}

iFileType *api::createFileType(size_t i)
{
   throw std::runtime_error("unimpled 16");
}

iFileTranslator *api::createFileTranslator(size_t fromFmt, size_t toFmt)
{
   return new fileTranslator(*this,toFmt);
}

iFont *api::createFont(const char *face, size_t size, COLORREF color, size_t options)
{
   throw std::runtime_error("unimpled 21");
}

void api::diagnostics()
{
   // implemented, but noop
}

fileTranslator::fileTranslator(api& a, size_t toFmt)
: subObject(a)
{
   using namespace Gdiplus;

   if(toFmt == iFileType::kPng)
      m_destId = { 0x557cf406, 0x1a04, 0x11d3,{ 0x9a,0x73,0x00,0x00,0xf8,0x1e,0xf3,0x2e } }; 
   else
      throw std::runtime_error("unsupported file type");
}

void fileTranslator::translate(const char *fromPath, const char *toPath)
{
   using namespace Gdiplus;

   Image image(widen(fromPath).c_str());
   auto success = image.Save(widen(toPath).c_str(),&m_destId,NULL);
   if(success != Ok)
      throw std::runtime_error("failed to save");
}

__declspec(dllexport) iGraphicsApi *create(iLog& l)
{
   return new api(l);
}
