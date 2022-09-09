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

iFont *api::createFont(const char *face, size_t size, size_t options)
{
   LOGFONTA lFont;
   fontFinder(lFont).findFirstInAnsiCharSet(face);

   // now, adjust for size and other options

   // size:
   // this magic is recommended by MSDN for MM_TEXT
   lFont.lfHeight = (::GetDeviceCaps(dc,LOGPIXELSY) * size) / 72;

   // options:
   lFont.lfWeight = (options & iFont::kBold) ? 700 : 400;
   lFont.lfItalic = (options & iFont::kItalic) ? TRUE : FALSE;
   lFont.lfUnderline = (options & iFont::kUnderline) ? TRUE : FALSE;
   lFont.lfStrikeOut = (options & iFont::kStrikeout) ? TRUE : FALSE;

   autoReleasePtr<font> pFont(new font(*this));
   pFont->hFont = ::CreateFontIndirectA(&lFont);
   if(!pFont->hFont)
      throw std::runtime_error("failed to create font");
   pFont->bkMode = (options & iFont::kOpaqueBackground) ? OPAQUE : TRANSPARENT;
   pFont->activate();
   return pFont.leak();
}

void fontFinder::findFirstInAnsiCharSet(const std::string& face)
{
   LOGFONTA lf;
   lf.lfCharSet = ANSI_CHARSET;
   ::strcpy(lf.lfFaceName,face.c_str());
   lf.lfPitchAndFamily = 0;

   auto dc = ::CreateCompatibleDC(NULL);
   ::EnumFontFamiliesExA(
      dc,                   // [in] HDC           hdc,
      &lf,                  // [in] LPLOGFONTA    lpLogfont,
      &fontFinder::onFound, // [in] FONTENUMPROCA lpProc,
      (LPARAM)this,         // [in] LPARAM        lParam,
      0                     // DWORD         dwFlags
   );
   ::DeleteDC(dc);

   if(!m_found)
      throw std::runtime_error("font not found!");
}

void fontFinder::onFound(const LOGFONTA& lf, const TEXTMETRICA& tm, DWORD type)
{
   ::memcpy(&m_lf,&lf,sizeof(LOGFONTA));
}

int fontFinder::onFound(const LOGFONTA *pLf, const TEXTMETRICA *pTm, DWORD ft, LPARAM lParam)
{
   ((fontFinder*)lParam)->onFound(*pLf,*pTm,ft);
   return 0; // abort
}

font::~font()
{
   if(hFont)
   {
      deactivate();
      ::DeleteObject(hFont);
   }
}

void font::activate()
{
   hOld = ::SelectObject(Api.dc,hFont);
}

void font::deactivate()
{
   ::SelectObject(Api.dc,hOld);
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
   if(hBmp)
   {
      deactivate();
      ::DeleteObject(hBmp);
   }
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
void bitmap::drawText(const point& p, const char *text, size_t flags, iFont& fnt)
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

void bitmap::activate()
{
   hOld = ::SelectObject(Api.dc,hBmp);
}

void bitmap::deactivate()
{
   ::SelectObject(Api.dc,hOld);
}

__declspec(dllexport) iGraphicsApi *create(iLog& l)
{
   return new api(l);
}
