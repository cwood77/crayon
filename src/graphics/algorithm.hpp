#pragma once
#include "graphicsApi.hpp"
#include <set>

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
