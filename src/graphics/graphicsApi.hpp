#pragma once
#define WIN32_LEAN_AND_MEAN
#include <cstdlib>
#include <windows.h>

class iLog;

class iBlock;

class iBlockFactory {
};

class iTransform; // i.e. rotation

class rect;

class iFont {
public:
   virtual ~iFont() {}
   virtual void addref() = 0;
   virtual void release() = 0;
};

class iCanvas {
public:
   virtual ~iCanvas() {}
   virtual void addref() = 0;
   virtual void release() = 0;

   // get/set pixel

   // draw text

   virtual iCanvas *subset(rect& r) = 0; // bounds/remaps coordinates

   virtual iBlock *copy(iBlockFactory& f, iTransform *pT) = 0; // rotate (or not)
   virtual void paste(iBlock *pB, size_t x, size_t y) = 0;
};

class iBitmap : public iCanvas {
public:
   virtual ~iBitmap() {}
   virtual void addref() = 0;
   virtual void release() = 0;

   // get color depth

   // get DPI
};

class iFileType {
public:
   virtual ~iFileType() {}
   virtual void addref() = 0;
   virtual void release() = 0;
   virtual iBitmap *loadBitmap(const char *path) = 0;
   virtual void saveBitmap(iBitmap& b, const char *path) = 0;
};

class iGraphicsApi {
public:
   virtual ~iGraphicsApi() {}
   virtual void addref() = 0;
   virtual void release() = 0;
   virtual iFileType *createFileType(size_t i) = 0;
   virtual iFont *createFont(const char *face, size_t size) = 0;
};

class graphicsApiFactory {
public:
   explicit graphicsApiFactory(iLog& l);
   ~graphicsApiFactory();

   iGraphicsApi *open(size_t i);

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

class objectFinder {
public:
   static void find(iCanvas& c, rect& r);
};

template<class T>
class autoReleasePtr {
public:
   explicit autoReleasePtr(T *pPtr = NULL) : m_pPtr(pPtr)
   { if(m_pPtr) m_pPtr->addref(); }

   void reset(T *pPtr = NULL)
   {
      if(pPtr)
         pPtr->addref();
      if(m_pPtr)
         m_pPtr->release();
      m_pPtr = pPtr;
   }

   operator bool() { return !!m_pPtr; }

   T *operator->() { return m_pPtr; }

private:
   T *m_pPtr;
};
