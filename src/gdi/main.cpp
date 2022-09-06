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

iSnippet *bitmap::snip(iSnippetAllocator& a, iTransform& t)
{
   long w = width;
   long h = height;
   t.translateDims(w,h);
   autoReleasePtr<iSnippet> pSnippet(a.allocate(w,h));

   for(long x=0;x<width;x++)
   {
      for(long y=0;y<height;y++)
      {
         COLORREF srcPix = ::GetPixel(Api.dc,x,y);

         point p(x,y);
         t.translateCoords(p);
         auto& destPix = pSnippet->index(p);

         destPix.set(srcPix);
      }
   }

   return pSnippet.leak();
}

void bitmap::overlay(iSnippet& s, COLORREF transparent)
{
   long w,h;
   s.getDims(w,h);
   for(long x=0;x<w;x++)
   {
      for(long y=0;y<h;y++)
      {
         auto& over = s.index(point(x,y));
         if(!over.is(transparent))
            ::SetPixel(Api.dc,x,y,over.toColorref());
      }
   }
}

__declspec(dllexport) iGraphicsApi *create(iLog& l)
{
   return new api(l);
}
