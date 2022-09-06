#include "api.hpp"

api::api(iLog& l)
: Log(l,"[gdi] "), dc(NULL)
{
   dc = ::CreateCompatibleDC(NULL);
   Log.s().s() << "created DC " << (size_t)dc << std::endl;
}

api::~api()
{
   Log.s().s() << "closing API" << std::endl;
   if(dc)
      ::DeleteDC(dc);
}

iFileType *api::createFileType(size_t i)
{
   if(i!=0)
      throw std::runtime_error("don't know this file type");
   return new bmpFileType(*this);
}

iBlock *bitmap::copy(iBlockFactory& f, iTransform *pT)
{
   throw std::runtime_error("unimpled 1");
}

__declspec(dllexport) iGraphicsApi *create(iLog& l)
{
   return new api(l);
}
