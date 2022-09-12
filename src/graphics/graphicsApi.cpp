#include "graphicsApi.hpp"
#include <stdexcept>
#include <string>

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

   m_api1 = ::LoadLibrary((tgt + "\\..\\gdiplusapi.dll").c_str());
   if(!m_api1)
      throw std::runtime_error("can't load GDI+ DLL");
   m_func1 = (fac_t)::GetProcAddress(m_api1,"_Z6createR4iLog");
   if(!m_func1)
      throw std::runtime_error("can't load GDI DLL+ (2)");
}

graphicsApiFactory::~graphicsApiFactory()
{
   if(!m_success) return;

   ::FreeLibrary(m_api1);
   ::FreeLibrary(m_api0);
}

iGraphicsApi *graphicsApiFactory::open(size_t i)
{
   if(i == 0)
      return m_func0(m_log);
   else if(i == 1)
      return m_func1(m_log);
   else
      throw std::runtime_error("API unsupported");
}

void graphicsApiFactory::diagnostics()
{
   {
      autoReleasePtr<iGraphicsApi> pApi(open(0));
      pApi->diagnostics();
   }
   {
      autoReleasePtr<iGraphicsApi> pApi(open(1));
      pApi->diagnostics();
   }
}
