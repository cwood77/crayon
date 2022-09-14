#pragma once
#include "graphicsApi.hpp"

// snippets have an allocator mainly so we're extra-sure we're using a compatible
// memory heap

class snippet : public iSnippet {
public:
   snippet(long w, long h);
   ~snippet();

   virtual void getDims(long& w, long& h) { w = m_w; h = m_h; }
   virtual pixel& index(const point& p);

private:
   long m_w;
   long m_h;
   pixel *m_pBlock;

cdwImplAddrefRelease();
};

class snippetAllocator : public iSnippetAllocator {
public:
   virtual iSnippet *allocate(long w, long h)
   { return new snippet(w,h); }
};

class nullTransform : public iTransform {
public:
   virtual void translateDims(long& w, long& h) {}
   virtual void translateCoords(point& p) {}
};

class clockwiseTransform : public iTransform {
public:
   explicit clockwiseTransform(double angle);
   virtual void translateDims(long& w, long& h);
   virtual void translateCoords(point& p);

private:
   void toCartesian(point& p);
   void toGraphics(point& p);

   double m_angle;
   point m_offset;
};
