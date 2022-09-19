#pragma once
#define WIN32_LEAN_AND_MEAN
#include <cstdlib>
#include <windows.h>

class iLog;

class point {
public:
   point(long _x, long _y) : x(_x), y(_y) {}

   long x;
   long y;
};

inline bool operator<(const point& lhs, const point& rhs)
{
   return (lhs.x == rhs.x) ? lhs.y < rhs.y : lhs.x < rhs.x;
}

class polPoint {
public:
   static polPoint fromCartesian(const point& p);
   point toCartesian() const;

   double r;
   double theta; // in radians
};

class rect {
public:
   rect() : x(0), y(0), w(0), h(0) {}
   rect(long _x, long _y, long _w, long _h) : x(_x), y(_y), w(_w), h(_h) {}

   void setOrigin(const point& p) { x = p.x; y = p.y; }
   void growToInclude(const point& p);
   RECT toRect() const;

   long x;
   long y;
   long w;
   long h;
};

#pragma pack(push,1)
class pixel {
public:
   bool is(COLORREF c) const
   {
      return
         0 == (0xFF000000 & c) &&
         r == GetRValue(c) &&
         g == GetGValue(c) &&
         b == GetBValue(c) ;
   }

   void set(COLORREF c)
   {
      r = GetRValue(c);
      g = GetGValue(c);
      b = GetBValue(c);
   }

   COLORREF toColorref()
   {
      return RGB(r,g,b);
   }

   unsigned char b;
   unsigned char g;
   unsigned char r;
};
#pragma pack(pop)

class iSnippet {
public:
   virtual ~iSnippet() {}
   virtual void addref() = 0;
   virtual void release() = 0;

   virtual void getDims(long& w, long& h) = 0;
   virtual pixel& index(const point& p) = 0;
};

class iSnippetAllocator {
public:
   virtual iSnippet *allocate(long w, long h);
};

class iTransform {
public:
   virtual void translateDims(long& w, long& h) = 0;
   virtual void translateCoords(point& p) = 0;
};

class iFont {
public:
   enum options {
      kItalic           = 0x1,
      kUnderline        = 0x2,
      kStrikeout        = 0x4,
      kOpaqueBackground = 0x8,
      kBold             = 0x10,
   };

   virtual ~iFont() {}
   virtual void addref() = 0;
   virtual void release() = 0;
};

// anything with dimension and pixels
// could be a bitmap, a snippet, or a subset of either
class iCanvas {
public:
   virtual ~iCanvas() {}
   virtual void addref() = 0;
   virtual void release() = 0;

   // get dims
   virtual void getDims(long& w, long& h) = 0;

   // get/set pixel
   virtual COLORREF getPixel(const point& p) = 0;
   virtual void setPixel(const point& p, COLORREF r) = 0;

   // draw text - can't be done on a snippet!
   virtual void drawText(const point& p, const char *text, size_t flags, iFont& font) = 0;
   virtual void drawText(const rect& r, const char *text, size_t flags, iFont& font) = 0;

   virtual iCanvas *subset(const rect& r) = 0; // bounds/remaps coordinates
   virtual iCanvas *superset() = 0;

   virtual iSnippet *snip(iSnippetAllocator& a, iTransform& t) = 0;
   //virtual void paste(iBlock *pB, size_t x, size_t y) = 0;

   virtual void overlay(iSnippet& s, COLORREF transparent) = 0;
};

class iBitmap : public iCanvas {
public:
   virtual ~iBitmap() {}
   virtual void addref() = 0;
   virtual void release() = 0;

   virtual void setDims(long w, long h) = 0;
};

class iFileType {
public:
   enum {
      kBmp = 0,
      kPng = 1,
   };

   virtual ~iFileType() {}
   virtual void addref() = 0;
   virtual void release() = 0;
   virtual iBitmap *loadBitmap(const char *path) = 0;
   virtual iBitmap *createNew(const rect& r, COLORREF c) = 0;
   virtual void saveBitmap(iBitmap& b, const char *path) = 0;
};

class iFileTranslator {
public:
   ~iFileTranslator() {}
   virtual void addref() = 0;
   virtual void release() = 0;
   virtual void translate(const char *fromPath, const char *toPath) = 0;
};

class iGraphicsApi {
public:
   virtual ~iGraphicsApi() {}
   virtual void addref() = 0;
   virtual void release() = 0;
   virtual iFileType *createFileType(size_t i) = 0;
   virtual iFileTranslator *createFileTranslator(size_t fromFmt, size_t toFmt) = 0;
   virtual iFont *createFont(const char *face, size_t size, COLORREF color, size_t options) = 0;
   virtual void diagnostics() = 0;
};

class graphicsApiFactory {
public:
   explicit graphicsApiFactory(iLog& l);
   ~graphicsApiFactory();

   iGraphicsApi *open(size_t i);

   void diagnostics();

   void markSuccess() { m_success = true; }

private:
   typedef iGraphicsApi *(*fac_t)(iLog& l);

   iLog& m_log;
   HMODULE m_api0;
   fac_t m_func0;
   HMODULE m_api1;
   fac_t m_func1;
   bool m_success;
};

template<class T>
class autoReleasePtr {
public:
   explicit autoReleasePtr(T *pPtr = NULL) : m_pPtr(pPtr), m_isTemp(false)
   { if(m_pPtr) m_pPtr->addref(); }

   ~autoReleasePtr()
   { reset(NULL); }

   void reset(T *pPtr = NULL)
   {
      if(pPtr)
         pPtr->addref();
      if(m_pPtr && m_isTemp)
      { m_pPtr->addref(); m_isTemp = false; }
      if(m_pPtr)
         m_pPtr->release();
      m_pPtr = pPtr;
   }

   T *get() { return m_pPtr; }

   operator bool() { return !!m_pPtr; }
   operator T&() { return *m_pPtr; }

   T *operator->() { return m_pPtr; }

   // be very careful with this...
   void holdTemp(T *pPtr)
   {
      if(m_pPtr)
         m_pPtr->release();
      m_pPtr = pPtr;
      m_isTemp = true;
   }

   T *leakTemp()
   {
      T *pRval = m_pPtr;
      m_pPtr = NULL;
      m_isTemp = false;
      return pRval;
   }

private:
   T *m_pPtr;
   bool m_isTemp;
};

class refCnter {
public:
   refCnter() : m_rc(0) {}

   void addref() { m_rc++; }
   bool release() { return --m_rc == 0; }

private:
   size_t m_rc;
};

#define cdwImplAddrefRelease() \
public: \
   virtual void addref() { m_rc.addref(); } \
   virtual void release() { if(m_rc.release()) delete this; } \
private: \
   refCnter m_rc;

// adjust b by +/-d but ceil/floor b at 255/0
BYTE adjByteBndChk(BYTE b, long d);

// misc
void drawBox(const RECT& r, COLORREF col, iCanvas& can);
void drawBox(const RECT& r, COLORREF col, COLORREF fillCol, iCanvas& can);

inline void drawBox(const rect& r, COLORREF col, iCanvas& can)
{ drawBox(r.toRect(),col,can); }
inline void drawBox(const rect& r, COLORREF col, COLORREF fillCol, iCanvas& can)
{ drawBox(r.toRect(),col,fillCol,can); }
