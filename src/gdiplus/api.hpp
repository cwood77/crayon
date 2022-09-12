#pragma once
#include "../crayon/log.hpp"
#include "../graphics/graphicsApi.hpp"

// these header are suggested for GDI+
// without at least some of them you get compile errors around PROPID etc.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "gdiplus.h"

class api : public iGraphicsApi {
public:
   explicit api(iLog& l);
   ~api();

   virtual iFileType *createFileType(size_t i);
   virtual iFileTranslator *createFileTranslator(size_t fromFmt, size_t toFmt);
   virtual iFont *createFont(const char *face, size_t size, COLORREF color, size_t options);
   virtual void diagnostics();

   class log Log;
   ULONG_PTR m_gdiPlusToken;

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

class fileTranslator : public iFileTranslator, public subObject {
public:
   fileTranslator(api& a, size_t toFmt);

   virtual void translate(const char *fromPath, const char *toPath);

private:
   CLSID m_destId;

cdwImplAddrefRelease();
};
