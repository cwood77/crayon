#pragma once
#include "graphicsApi.hpp"
#include <list>
#include <map>
#include <memory>
#include <set>

class iSymbol;
class log;
class iSymbolTable;

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

class notPixelCriteria : public iPixelCriteria {
public:
   explicit notPixelCriteria(iPixelCriteria& inner) : m_pInner(&inner) {}

   virtual bool isEligible(COLORREF c) { return !m_pInner->isEligible(c); }

private:
   std::unique_ptr<iPixelCriteria> m_pInner;
};

class isPixelCriteria : public iPixelCriteria {
public:
   explicit isPixelCriteria(COLORREF x) : m_x(x) {}

   virtual bool isEligible(COLORREF c) { return m_x == c; }

private:
   COLORREF m_x;
};

class inColorRangeCriteria : public iPixelCriteria {
public:
   explicit inColorRangeCriteria(const colorRange& cr) : m_cr(cr) {}

   virtual bool isEligible(COLORREF c);

private:
   colorRange m_cr;
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

   // defined as the rect with the outermost bounds of any point inside the frame
   rect calculateInside();

private:
   bool isAdjacentPixelIn(const point& p, bool later = false);
   void reconsiderLater(const point& p);
   void reconsider();

   iCanvas& m_canvas;
   long m_w;
   long m_h;
   std::set<point> m_inFrame;
   std::set<point> m_later;
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

class objectSurvey {
public:
   objectSurvey(iCanvas& c, log& l);

   size_t getNumFoundObjects() const { return m_objects.size(); }

   rect& findObject(size_t n);
   rect& findObjectByTag(const std::string& tag);
   rect superset();

   void consumeTags() { m_consumeTags = true; }
   std::string getTag(size_t n);

private:
   void run();
   size_t findAdjacentMembership(const point& p);
   void addToObject(const point& p, size_t i);
   size_t mergeObjectsIf(size_t oldObj, size_t newObj);

   void makeBounds(size_t bndsIdx, size_t objId);
   void makeBounds(size_t id, const point& p);

   void storeAndTrimTagIf(size_t bndsIdx, size_t objId);
   size_t countPixels(iCanvas& c, long y, long w);

   iCanvas& m_canvas;
   log& m_log;
   long m_w;
   long m_h;

   size_t m_nextObjId;
   std::map<size_t,std::set<point> > m_objects;
   std::map<point,size_t> m_map;
   std::map<size_t,long> m_objectTagHeight;
   std::map<size_t,rect> m_bounds;
   bool m_consumeTags;
   std::map<size_t,std::string> m_tags;
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
   void dupWhiskerError(bool h, COLORREF col, long old, long nu);

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

class toColor : public iPixelTransform {
public:
   toColor(iPixelCriteria& c, COLORREF destCol)
   : m_c(c), m_destCol(destCol) {}

   virtual COLORREF run(COLORREF c) { return m_c.isEligible(c) ? m_destCol : c; }

private:
   iPixelCriteria& m_c;
   COLORREF m_destCol;
};

class pixelTransformer {
public:
   pixelTransformer(iCanvas& c, log& l) : m_c(c), m_l(l) {}

   void run(iPixelTransform& t);

private:
   iCanvas& m_c;
   log& m_l;
};

class iPixelAnalysis {
public:
   virtual void addPixel(COLORREF c) = 0;
   virtual iSymbol& complete() = 0;
};

class unattestedColorFinder : public iPixelAnalysis {
public:
   explicit unattestedColorFinder(COLORREF c) : m_col(c) {}

   virtual void addPixel(COLORREF c) { m_attested.insert(c); }
   virtual iSymbol& complete();

private:
   COLORREF m_col;
   std::set<COLORREF> m_attested;
};

class pixelAnalyzer {
public:
   pixelAnalyzer(iCanvas& c, iSymbolTable& s, log& l)
   : m_c(c), m_sTable(s), m_l(l) {}

   void run(iPixelAnalysis& a, const std::string& varName);

private:
   iCanvas& m_c;
   iSymbolTable& m_sTable;
   log& m_l;
};

class tagWriter {
public:
   static const COLORREF tagBgCol;

   tagWriter(iCanvas& c, log& l) : m_c(c), m_l(l) {}

   long write(const std::string& text);
   std::string readIf();

private:
   iCanvas& m_c;
   log& m_l;
};

class tagReader {
public:
   tagReader(iCanvas& c, log& l) : m_c(c), m_l(l) {}

   std::string readIf();

private:
   static char getTagChar(COLORREF c, bool& isValid);

   iCanvas& m_c;
   log& m_l;
};
