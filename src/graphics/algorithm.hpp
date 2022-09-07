#pragma once
#include "graphicsApi.hpp"
#include <map>
#include <set>

class log;

class frameRemover {
public:
   static void run(iCanvas& c);

private:
   explicit frameRemover(iCanvas& c);

   void _run();
   bool isAdjacentPixelIn(const point& p);
   void markIn(const point& p);

   iCanvas& m_canvas;
   long m_w;
   long m_h;
   std::set<point> m_inFrame;
   COLORREF m_frameColor;
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
