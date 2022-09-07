#pragma once
#define WIN32_LEAN_AND_MEAN
#include "../crayon/log.hpp"
#include "../graphics/graphicsApi.hpp"
#include "windows.h"
#include <stdexcept>

class api : public iGraphicsApi {
public:
   explicit api(iLog& l);
   ~api();

   virtual iFileType *createFileType(size_t i);

   /*virtual iFont *createFont(const char *face, size_t size)
   { throw std::runtime_error("font not yet supported"); }*/

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

class canvas : public iCanvas, public subObject {
public:
   canvas(api& a, iCanvas& c, const rect& dims);
   virtual void getDims(long& w, long& h) { w = m_dims.w; h = m_dims.h; }
   virtual COLORREF getPixel(const point& p) { return m_pInner->getPixel(translate(p)); }
   virtual void setPixel(const point& p, COLORREF r) { m_pInner->setPixel(translate(p),r); }
   virtual iCanvas *subset(const rect& r) { return new canvas(Api,*this,r); }
   virtual iSnippet *snip(iSnippetAllocator& a, iTransform& t) { return Snip(a,t,*this); }
   virtual void overlay(iSnippet& s, COLORREF transparent) { Overlay(s,transparent,*this); }

   static iSnippet *Snip(iSnippetAllocator& a, iTransform& t, iCanvas& c);
   static void Overlay(iSnippet& s, COLORREF transparent, iCanvas& c);

private:
   point translate(const point& p) { return point(m_dims.x+p.x,m_dims.y+p.y); }

   rect m_dims;
   autoReleasePtr<iCanvas> m_pInner;

cdwImplAddrefRelease();
};

class bitmap : public iBitmap, public subObject {
public:
   virtual ~bitmap();
   virtual void getDims(long& w, long& h) { w = width; h = height; }
   virtual COLORREF getPixel(const point& p);
   virtual void setPixel(const point& p, COLORREF r);
   virtual iCanvas *subset(const rect& r) { return new canvas(Api,*this,r); }
   virtual iSnippet *snip(iSnippetAllocator& a, iTransform& t)
   { return canvas::Snip(a,t,*this); }
   virtual void overlay(iSnippet& s, COLORREF transparent)
   { canvas::Overlay(s,transparent,*this); }

   long width;
   long height;

cdwImplSubObject(bitmap);
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
