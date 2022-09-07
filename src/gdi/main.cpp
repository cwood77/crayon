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

canvas::canvas(api& a, iCanvas& c, const rect& dims)
: subObject(a), m_dims(dims), m_pInner(&c)
{
   m_dims.w -= m_dims.x;
   m_dims.x = 0;
   m_dims.h -= m_dims.y;
   m_dims.y = 0;
}

iSnippet *canvas::Snip(iSnippetAllocator& a, iTransform& t, iCanvas& c)
{
   long w,h;
   c.getDims(w,h);
   t.translateDims(w,h);
   autoReleasePtr<iSnippet> pSnippet(a.allocate(w,h));

   for(long x=0;x<w;x++)
   {
      for(long y=0;y<h;y++)
      {
         COLORREF srcPix = c.getPixel(point(x,y));

         point p(x,y);
         t.translateCoords(p);
         auto& destPix = pSnippet->index(p);

         destPix.set(srcPix);
      }
   }

   return pSnippet.leak();
}

void canvas::Overlay(iSnippet& s, COLORREF transparent, iCanvas& c)
{
   long w,h;
   s.getDims(w,h);
   for(long x=0;x<w;x++)
   {
      for(long y=0;y<h;y++)
      {
         auto& over = s.index(point(x,y));
         if(!over.is(transparent))
            c.setPixel(point(x,y),over.toColorref());
      }
   }
}

bitmap::~bitmap()
{
   Api.Log.s().s() << "bitmap closing" << std::endl;
}

COLORREF bitmap::getPixel(const point& p)
{
   return ::GetPixel(Api.dc,p.x,p.y);
}

void bitmap::setPixel(const point& p, COLORREF r)
{
   ::SetPixel(Api.dc,p.x,p.y,r);
}

__declspec(dllexport) iGraphicsApi *create(iLog& l)
{
   return new api(l);
}
