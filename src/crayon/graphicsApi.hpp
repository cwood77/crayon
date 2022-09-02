#pragma once
#include <cstdlib>

class iBlock;

class iBlockFactory {
};

class iTransform; // i.e. rotation

class rect;

class iFont {
public:
   virtual ~iFont() {}
};

class iCanvas {
public:
   // get/set pixel

   // draw text

   virtual iCanvas *subset(rect& r) = 0; // bounds/remaps coordinates

   virtual iBlock *copy(iBlockFactory& f, iTransform *pT) = 0; // rotate (or not)
   virtual void paste(iBlock *pB, size_t x, size_t y) = 0;
};

class iBitmap : public iCanvas {
public:
   virtual ~iBitmap() {}

   // get color depth

   // get DPI
};

class iFileType {
public:
   virtual ~iFileType() {}
   virtual iBitmap *loadBitmap(const char *path) = 0;
   virtual void saveBitmap(iBitmap& b, const char *path) = 0;
};

class iGraphicsApi {
public:
   virtual ~iGraphicsApi() {}
   virtual iFileType *createFileType() = 0;
   virtual iFont *createFont(const char *face, size_t size) = 0;
};

class graphicsApiFactory {
public:
   iGraphicsApi *open();
};

class objectFinder {
public:
   static void find(iCanvas& c, rect& r);
};
