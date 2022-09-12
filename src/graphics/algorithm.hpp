#pragma once
#include "graphicsApi.hpp"
#include <list>
#include <map>
#include <set>

class log;

class iPixelCriteria {
public:
   virtual bool isEligible(COLORREF c) = 0;
};

class lightnessPixelCriteria : public iPixelCriteria {
public:
   explicit lightnessPixelCriteria(double minLightness) : m_minLightness(minLightness) {}

   virtual bool isEligible(COLORREF c);

private:
   double m_minLightness;
};

class framer {
public:
   explicit framer(iCanvas& c);

   void inferFrameColorFromOrigin();
   void initFrameColor(COLORREF c);

   void findFrame();
   void colorFrame(COLORREF c);

   void calculateOutline(
      // last pixels of the frame
      std::set<point>& frameEdge,
      // pixels just outside the frame
      std::set<point>& offEdge);
   COLORREF getFrameColor() { return m_frameColor; }
   void markIn(const point& p);
   void unmark(const point& p);

private:
   bool isAdjacentPixelIn(const point& p);

   iCanvas& m_canvas;
   long m_w;
   long m_h;
   std::set<point> m_inFrame;
   COLORREF m_frameColor;
};

class outliner {
public:
   explicit outliner(iCanvas& c, framer& f, log& Log)
   : m_c(c), m_f(f), m_log(Log) {}

   void encroach(iPixelCriteria& c, COLORREF col);
   void retreat(COLORREF c);

private:
   iCanvas& m_c;
   framer& m_f;
   log& m_log;
};

class objectFinder {
public:
   static rect run(iCanvas& c, size_t n, bool dbgHilight, log& Log);

private:
   objectFinder(iCanvas& c, log& l);

   rect _run(size_t n, bool dbgHilight);
   size_t findAdjacentMembership(const point& p);
   void addToObject(const point& p, size_t i);
   size_t mergeObjectsIf(size_t oldObj, size_t newObj);

   void makeBounds(size_t id, const point& p);

   void hilight(const rect& r, COLORREF c);

   iCanvas& m_canvas;
   log& m_log;
   long m_w;
   long m_h;

   size_t m_nextObjId;
   std::map<size_t,std::set<point> > m_objects;
   std::map<point,size_t> m_map;
   std::map<size_t,rect> m_bounds;
};

class whiskerSurvey {
public:
   enum { kCenter = 0xFFFFFFFF };

   whiskerSurvey(iCanvas& c, log& l) : m_canvas(c), m_log(l), m_center(0,0)
   { categorize(); }

   point findPoint(COLORREF x, COLORREF y);

   void clear();

private:
   void categorize();
   void categorizeVert();
   void categorizeHoriz();
   void markVertWhisker(long x, long y, COLORREF c);
   void markHorizWhisker(long x, long y, COLORREF c);

   iCanvas& m_canvas;
   log& m_log;

   std::list<point> m_whiskerPoints;
   std::map<COLORREF,long> m_vertWhiskers;
   std::map<COLORREF,long> m_horizWhiskers;
   point m_center;
};

class iPixelTransform {
public:
   virtual COLORREF run(COLORREF c) = 0;
};

class componentShift : public iPixelTransform {
public:
   componentShift(char c, long s) : m_c(c), m_s(s) {}

   virtual COLORREF run(COLORREF c);

private:
   BYTE diffMin0(long c, long x)
   { return c > x ? c - x : 0; }
   BYTE addMax255(long c, long x)
   { return c+x < 255 ? c+x : 255; }

   char m_c;
   long m_s;
};

class lightnessShift : public iPixelTransform {
public:
   explicit lightnessShift(long s) : m_s(s) {}

   virtual COLORREF run(COLORREF c);

private:
   long m_s;
};

class toMonochromeShift : public iPixelTransform {
public:
   explicit toMonochromeShift(iPixelCriteria& c)
   : m_c(c) {}

   virtual COLORREF run(COLORREF c);

private:
   iPixelCriteria& m_c;
};

class pixelTransformer {
public:
   pixelTransformer(iCanvas& c, log& l) : m_c(c), m_l(l) {}

   void run(iPixelTransform& t);

private:
   iCanvas& m_c;
   log& m_l;
};
