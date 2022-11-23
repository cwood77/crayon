#include "../../crayon/log.hpp"
#include "../algorithm.hpp"

framer::framer(iCanvas& c)
: m_canvas(c)
{
   m_canvas.getDims(m_w,m_h);
}

void framer::inferFrameColorFromOrigin()
{
   m_frameColor = m_canvas.getPixel(point(0,0));
   markIn(point(0,0));
}

void framer::initFrameColor(COLORREF c)
{
   m_frameColor = c;
}

void framer::findFrame()
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
         else
            reconsiderLater(point(x,y));
      }
   }
   reconsider();
}

void framer::colorFrame(COLORREF c)
{
   for(auto pt : m_inFrame)
      m_canvas.setPixel(pt,c);
   m_frameColor = c;
}

void framer::calculateOutline(std::set<point>& frameEdge, std::set<point>& offEdge)
{
   for(auto pt : m_inFrame)
   {
      // disregard points on the edges of the canvas
      if(pt.x == 0 || pt.x == 0+m_w-1)
         continue;
      if(pt.y == 0 || pt.y == 0+m_h-1)
         continue;

      // calculate cardinally adjacent points
      std::list<point> candidates;
      candidates.push_back(point(pt.x-1,pt.y));
      candidates.push_back(point(pt.x+1,pt.y));
      candidates.push_back(point(pt.x,pt.y-1));
      candidates.push_back(point(pt.x,pt.y+1));

      // any of those that aren't in the frame are consisdered part of the outline
      for(auto can : candidates)
      {
         if(m_inFrame.find(can)==m_inFrame.end())
         {
            frameEdge.insert(pt);
            offEdge.insert(can);
         }
      }
   }
}

void framer::markIn(const point& p)
{
   m_inFrame.insert(p);
}

void framer::unmark(const point& p)
{
   m_inFrame.erase(p);
}

bool framer::isAdjacentPixelIn(const point& p, bool later)
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
      if(it!=m_inFrame.end())
         return true;
   }
   if(later)
   {
      // on the second pass, we can look below and to the right
      {
         auto it = m_inFrame.find(point(p.x,p.y+1));
         if(it!=m_inFrame.end())
            return true;
      }
      {
         auto it = m_inFrame.find(point(p.x+1,p.y));
         if(it!=m_inFrame.end())
            return true;
      }
   }
   return false;
}

void framer::reconsiderLater(const point& p)
{
   m_later.insert(p);
}

void framer::reconsider()
{
   bool progress = false;
   do
   {
      progress = false;
      auto copy = m_later;
      for(auto pt : copy)
      {
         if(isAdjacentPixelIn(pt,/*later=*/true))
         {
            m_inFrame.insert(pt);
            m_later.erase(pt);
            progress = true;
         }
      }
   } while(progress);
   m_later.clear();
}
