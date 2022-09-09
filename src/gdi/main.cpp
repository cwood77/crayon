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

// there is both horizontal and vertical slop injected by DrawText..?
// I account for the vertical slop with GetTextMetrics below; but I'm
// not sure how to account for horizontal slop, or that it's worth trying.
void bitmap::drawText(const point& p, const char *text, size_t flags)
{
   const size_t hugeSize = 1000;
   RECT r;
   r.top = p.y - hugeSize;
   r.bottom = p.y;
   if(flags & DT_CENTER)
   {
      r.left = p.x - hugeSize;
      r.right = p.x + hugeSize;
   }
   else
   {
      r.left = p.x;
      r.right = p.x + hugeSize;
   }

   // coords passed in are assumed to be the line you're writing on, like
   // in elementary school.  That means the 'descent' (i.e. the parts of letters
   // like "j" that hang below the line) needs to be added.
   TEXTMETRICA tInfo;
   ::GetTextMetrics(Api.dc,&tInfo);
   r.bottom += tInfo.tmDescent;

   auto success = ::DrawText(
      Api.dc,
      text,
      -1,
      &r,
      flags);
   if(!success)
      throw std::runtime_error("draw text failed");
}

__declspec(dllexport) iGraphicsApi *create(iLog& l)
{
   return new api(l);
}
