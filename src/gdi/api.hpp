#pragma once
#define WIN32_LEAN_AND_MEAN
#include "../crayon/log.hpp"
#include "../graphics/graphicsApi.hpp"
#include "windows.h"
#include <map>
#include <stdexcept>

class api : public iGraphicsApi {
public:
   explicit api(iLog& l);
   ~api();

   virtual iFileType *createFileType(size_t i);
   virtual iFont *createFont(const char *face, size_t size, size_t options);
   virtual void diagnostics();

   log Log;
   HDC dc;

cdwImplAddrefRelease();
};

class subObject {
public:
   subObject(api& a) : Api(a) {}
   virtual ~subObject() {}

   api& Api;
};

#define cdwImplSubObject(__type__) \
public: \
   __type__(api& a) : subObject(a) {}

class fontFinder {
public:
   explicit fontFinder(LOGFONTA& a) : m_lf(a), m_found(false) {}
   void findInAnsiCharSet(const std::string& face, size_t options, log& Log);

private:
   void onFound(const LOGFONTA& lf, const TEXTMETRICA& tm, DWORD type);
   static int onFound(const LOGFONTA *pLf, const TEXTMETRICA *pTm, DWORD ft, LPARAM lParam);
   size_t computeScore(const LOGFONTA& lf);

   LOGFONTA& m_lf;
   std::map<size_t,LOGFONTA> m_matches;
   bool m_found;
   size_t m_options;
};

class font : public iFont, public subObject {
public:
   explicit font(api& a) : subObject(a), hFont(NULL) {}
   ~font();
   void activate();
   void deactivate();

   HFONT hFont;
   INT bkMode;

private:
   HGDIOBJ hOld;

cdwImplAddrefRelease();
};

class dpiAdjuster {
public:
   static void notifyAwareProcess(log& Log);

   dpiAdjuster& scale(long& v);

private:
   static double m_scale;
};

class autoBackgroundMode {
public:
   autoBackgroundMode(HDC hdc, int mode);
   ~autoBackgroundMode();

private:
   HDC m_dc;
   int m_oldMode;
};

class canvas : public iCanvas, public subObject {
public:
   canvas(api& a, iCanvas& c, const rect& dims);
   virtual void getDims(long& w, long& h) { w = m_dims.w; h = m_dims.h; }
   virtual COLORREF getPixel(const point& p) { return m_pInner->getPixel(translate(p)); }
   virtual void setPixel(const point& p, COLORREF r) { m_pInner->setPixel(translate(p),r); }
   virtual void drawText(const point& p, const char *text, size_t flags, iFont& font)
   { m_pInner->drawText(translate(p),text,flags,font); }
   virtual void drawText(const rect& r, const char *text, size_t flags, iFont& font)
   { m_pInner->drawText(translate(r),text,flags,font); }
   virtual iCanvas *subset(const rect& r) { return new canvas(Api,*this,r); }
   virtual iCanvas *superset() { return m_pInner.get(); }
   virtual iSnippet *snip(iSnippetAllocator& a, iTransform& t) { return Snip(a,t,*this); }
   virtual void overlay(iSnippet& s, COLORREF transparent) { Overlay(s,transparent,*this); }

   static iSnippet *Snip(iSnippetAllocator& a, iTransform& t, iCanvas& c);
   static void Overlay(iSnippet& s, COLORREF transparent, iCanvas& c);

private:
   point translate(const point& p) { return point(m_dims.x+p.x,m_dims.y+p.y); }
   rect translate(const rect& r) { return rect(m_dims.x+r.x,m_dims.y+r.y,r.w,r.h); }

   rect m_dims;
   autoReleasePtr<iCanvas> m_pInner;

cdwImplAddrefRelease();
};

class bitmap : public iBitmap, public subObject {
public:
   explicit bitmap(api& a) : subObject(a), hBmp(NULL) {}
   virtual ~bitmap();
   virtual void getDims(long& w, long& h) { w = width; h = height; }
   virtual COLORREF getPixel(const point& p);
   virtual void setPixel(const point& p, COLORREF r);
   virtual void drawText(const point& p, const char *text, size_t flags, iFont& font);
   virtual void drawText(const rect& p, const char *text, size_t flags, iFont& font);
   virtual iCanvas *subset(const rect& r) { return new canvas(Api,*this,r); }
   virtual iCanvas *superset() { return this; }
   virtual iSnippet *snip(iSnippetAllocator& a, iTransform& t)
   { return canvas::Snip(a,t,*this); }
   virtual void overlay(iSnippet& s, COLORREF transparent)
   { canvas::Overlay(s,transparent,*this); }
   virtual void setDims(long w, long h) { width = w; height = h; }

   void activate();
   void deactivate();

   long width;
   long height;
   HBITMAP hBmp;

private:
   void drawText(const RECT& gdiR, int bkMode, const char *text, size_t flags);

   HGDIOBJ hOld;

cdwImplAddrefRelease();
};

class bmpFileType : public iFileType, public subObject {
public:
   virtual iBitmap *loadBitmap(const char *path);
   virtual void saveBitmap(iBitmap& b, const char *path);

private:
   void populateStructs(long w, long h, BITMAPFILEHEADER& fHdr, BITMAPINFOHEADER& iHdr);
   void checkCompatible(BITMAPFILEHEADER& hdr);
   void checkCompatible(BITMAPINFOHEADER& hdr);
   DWORD calculateMagicSize(long w, long h);

cdwImplSubObject(bmpFileType);
cdwImplAddrefRelease();
};

// misc
void drawBox(const RECT& r, COLORREF col, iCanvas& can);
