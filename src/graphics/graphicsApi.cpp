#include "graphicsApi.hpp"
#include <stdexcept>
#include <string>

void rect::growToInclude(const point& p)
{
   // X
   if(p.x < x)
   {
      w += (x-p.x);
      x = p.x;
   }
   else if(p.x > x+w-1)
      w += (p.x - (x+w-1));

   // Y
   if(p.y < y)
   {
      h += (y-p.y);
      y = p.y;
   }
   else if(p.y > y+h-1)
      h += (p.y - (y+h-1));
}

graphicsApiFactory::graphicsApiFactory(iLog& l)
: m_log(l)
, m_api0(NULL)
, m_func0(NULL)
, m_api1(NULL)
, m_func1(NULL)
, m_success(false)
{
   char buffer[MAX_PATH];
   ::GetModuleFileNameA(NULL,buffer,MAX_PATH);

   std::string tgt = buffer;
   m_api0 = ::LoadLibrary((tgt + "\\..\\gdiapi.dll").c_str());
   if(!m_api0)
      throw std::runtime_error("can't load GDI DLL");
   m_func0 = (fac_t)::GetProcAddress(m_api0,"_Z6createR4iLog");
   if(!m_func0)
      throw std::runtime_error("can't load GDI DLL (2)");
}

graphicsApiFactory::~graphicsApiFactory()
{
   if(m_success)
      ::FreeLibrary(m_api0);
}

iGraphicsApi *graphicsApiFactory::open(size_t i)
{
   if(i != 0)
      throw std::runtime_error("API unsupported");

   return m_func0(m_log);
}
