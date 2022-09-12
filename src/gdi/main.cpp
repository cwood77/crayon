#include "api.hpp"

api::api(iLog& l)
: Log(l,"[gdi] "), dc(NULL)
{
   dpiAdjuster::notifyAwareProcess(Log);
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

iFont *api::createFont(const char *face, size_t size, COLORREF color, size_t options)
{
   LOGFONTA lFont;
   fontFinder(lFont).findInAnsiCharSet(face,options,Log);

   // now, adjust for size and other options

   // capture ratio
   double aspectRatio = lFont.lfWidth / lFont.lfHeight;

   // size:
   // this magic is recommended by MSDN for MM_TEXT
   lFont.lfHeight = (::GetDeviceCaps(dc,LOGPIXELSY) * size) / 72.0;
   lFont.lfWidth = lFont.lfHeight * aspectRatio;
   lFont.lfHeight *= -1; // ?

   // DPI shenanigans
   //dpiAdjuster().scale(lFont.lfHeight).scale(lFont.lfWidth);

   // options:
   lFont.lfWeight = (options & iFont::kBold) ? 700 : 400;
   lFont.lfItalic = (options & iFont::kItalic) ? TRUE : FALSE;
   lFont.lfUnderline = (options & iFont::kUnderline) ? TRUE : FALSE;
   lFont.lfStrikeOut = (options & iFont::kStrikeout) ? TRUE : FALSE;

   autoReleasePtr<font> pFont;
   pFont.holdTemp(new font(*this));
   pFont->hFont = ::CreateFontIndirectA(&lFont);
   if(!pFont->hFont)
      throw std::runtime_error("failed to create font");
   pFont->color = color;
   pFont->bkMode = (options & iFont::kOpaqueBackground) ? OPAQUE : TRANSPARENT;
   pFont->activate();
   return pFont.leakTemp();
}

void fontFinder::findInAnsiCharSet(const std::string& face, size_t options, log& Log)
{
   LOGFONTA lf;
   lf.lfCharSet = ANSI_CHARSET;
   ::strcpy(lf.lfFaceName,face.c_str());
   lf.lfPitchAndFamily = 0;

   m_options = options;

   auto dc = ::CreateCompatibleDC(NULL);
   ::EnumFontFamiliesExA(
      dc,                   // [in] HDC           hdc,
      &lf,                  // [in] LPLOGFONTA    lpLogfont,
      &fontFinder::onFound, // [in] FONTENUMPROCA lpProc,
      (LPARAM)this,         // [in] LPARAM        lParam,
      0                     // DWORD         dwFlags
   );
   ::DeleteDC(dc);

   if(!m_matches.size())
      throw std::runtime_error("font not found!");

   auto it = --m_matches.end();
   Log.s().s() << "matching font with score " << it->first << " of " << m_matches.size() << " candidates" << std::endl;
   ::memcpy(&m_lf,&it->second,sizeof(LOGFONTA));
}

void fontFinder::onFound(const LOGFONTA& lf, const TEXTMETRICA& tm, DWORD type)
{
   m_matches[computeScore(lf)] = lf;
}

int fontFinder::onFound(const LOGFONTA *pLf, const TEXTMETRICA *pTm, DWORD ft, LPARAM lParam)
{
   ((fontFinder*)lParam)->onFound(*pLf,*pTm,ft);
   return 1;
}

size_t fontFinder::computeScore(const LOGFONTA& lf)
{
   BOOL i = (m_options & iFont::kItalic) ? TRUE : FALSE;
   BOOL u = (m_options & iFont::kUnderline) ? TRUE : FALSE;
   BOOL s = (m_options & iFont::kStrikeout) ? TRUE : FALSE;
   BOOL w = (m_options & iFont::kBold) ? 700 : 400;

   return
      ((lf.lfWeight == w)    ? 1000 : 0) +
      ((lf.lfItalic == i)    ?  100 : 0) +
      ((lf.lfUnderline == u) ?   10 : 0) +
      ((lf.lfStrikeOut == s) ?   10 : 0) +
      0;
}

font::~font()
{
   Api.Log.s().s() << "font closing" << std::endl;
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

double dpiAdjuster::m_scale = 1.0;

void dpiAdjuster::notifyAwareProcess(log& Log)
{
   ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
   auto dpi = ::GetDpiForSystem();
   m_scale = dpi / 96.0;
   Log.s().s() << "scaling for DPI is " << m_scale << "x (" << dpi << " dpi)" << std::endl;
}

dpiAdjuster& dpiAdjuster::scale(long& v)
{
   v *= m_scale;
   return *this;
}

autoTextColor::autoTextColor(HDC hdc, COLORREF newColor)
: m_dc(hdc), m_valid(true)
{
   if(newColor != 0xFFFFFFFF)
      m_oldColor = ::SetTextColor(m_dc,newColor);
   else
      m_valid = false;
}

autoTextColor::~autoTextColor()
{
   if(m_valid)
      ::SetTextColor(m_dc,m_oldColor);
}

autoBackgroundMode::autoBackgroundMode(HDC hdc, int mode)
: m_dc(hdc)
{
   m_oldMode = ::GetBkMode(m_dc);
   ::SetBkMode(m_dc,mode);
}

autoBackgroundMode::~autoBackgroundMode()
{
   ::SetBkMode(m_dc,m_oldMode);
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
   autoReleasePtr<iSnippet> pSnippet;
   pSnippet.holdTemp(a.allocate(w,h));

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

   return pSnippet.leakTemp();
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
void bitmap::drawText(const point& p, const char *text, size_t flags, iFont& _fnt)
{
   font& fnt = dynamic_cast<font&>(_fnt);

   // p describes the bottom, left/center point in a huge rect
   const size_t hugeSize = 100000;
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

   drawText(r,fnt.color,fnt.bkMode,text,flags);
}

void bitmap::drawText(const rect& p, const char *text, size_t flags, iFont& _fnt)
{
   font& fnt = dynamic_cast<font&>(_fnt);
   drawText(p.toRect(),fnt.color,fnt.bkMode,text,flags);
}

void bitmap::activate()
{
   hOld = ::SelectObject(Api.dc,hBmp);
}

void bitmap::deactivate()
{
   ::SelectObject(Api.dc,hOld);
}

void bitmap::drawText(const RECT& gdiR, COLORREF color, int bkMode, const char *text, size_t flags)
{
   // occasional debugging
   //::drawBox(gdiR,RGB(0,255,0),*this);

   RECT copy = gdiR;
   autoTextColor _tCol(Api.dc,color);
   autoBackgroundMode _bgm(Api.dc,bkMode);
   auto success = ::DrawText(
      Api.dc,
      text,
      -1,
      &copy,
      flags);
   if(!success)
      throw std::runtime_error("draw text failed");
}

void drawBox(const RECT& r, COLORREF col, iCanvas& can)
{
   for(long x=r.left;x<=r.right;x++)
   {
      can.setPixel(point(x,r.top),col);
      can.setPixel(point(x,r.bottom),col);
   }
   for(long y=r.top;y<=r.bottom;y++)
   {
      can.setPixel(point(r.left,y),col);
      can.setPixel(point(r.right,y),col);
   }
}

__declspec(dllexport) iGraphicsApi *create(iLog& l)
{
   return new api(l);
}
