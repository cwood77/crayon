#include "algorithm.hpp"

void frameRemover::run(iCanvas& c)
{
   frameRemover self(c);
   self._run();
}

frameRemover::frameRemover(iCanvas& c)
: m_canvas(c)
{
   m_canvas.getDims(m_w,m_h);
   m_frameColor = m_canvas.getPixel(point(0,0));
   markIn(point(0,0));
}

void frameRemover::_run()
{
   for(long y=0;y<m_h;y++)
   {
      for(long x=0;x<m_w;x++)
      {
         auto col = m_canvas.getPixel(point(x,y));
         if(col != m_frameColor)
            continue;

         if(isAdjacentPixelIn(point(x,y)))
            markIn(point(x,y));
      }
   }
}

bool frameRemover::isAdjacentPixelIn(const point& p)
{
   if(p.x)
   {
      auto it = m_inFrame.find(point(p.x-1,p.y));
      if(it!=m_inFrame.end())
         return true;
   }
   if(p.y)
   {
      auto it = m_inFrame.find(point(p.x,p.y-1));
      return it!=m_inFrame.end();
   }
   return false;
}

void frameRemover::markIn(const point& p)
{
   m_canvas.setPixel(p,RGB(255,255,255));
   m_inFrame.insert(p);
}
