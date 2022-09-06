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

class bitmap : public iBitmap, public subObject {
public:
   virtual void getDims(long& w, long& h) { w = width; h = height; }
   virtual COLORREF getPixel(const point& p);
   virtual void setPixel(const point& p, COLORREF r);
   virtual iSnippet *snip(iSnippetAllocator& a, iTransform& t);
   virtual void overlay(iSnippet& s, COLORREF transparent);

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
