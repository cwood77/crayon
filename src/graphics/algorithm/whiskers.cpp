#include "../../crayon/log.hpp"
#include "../algorithm.hpp"

point whiskerSurvey::findPoint(COLORREF x, COLORREF y)
{
   m_log.s().s() << "find (" << x << "," << y << ")" << std::endl;

   point rval(0,0);

   if(x == kCenter)
      rval.x = m_center.x;
   else
   {
      auto vit = m_vertWhiskers.find(x);
      if(vit == m_vertWhiskers.end())
         throw std::runtime_error("X whisker not found");
      rval.x = vit->second;
   }

   if(y == kCenter)
      rval.y = m_center.y;
   else
   {
      auto hit = m_horizWhiskers.find(y);
      if(hit == m_horizWhiskers.end())
         throw std::runtime_error("Y whisker not found");
      rval.y = hit->second;
   }

   return rval;
}

void whiskerSurvey::clear()
{
   auto col = RGB(255,255,255);

   for(auto pt : m_whiskerPoints)
      m_canvas.setPixel(pt,col);
}

void whiskerSurvey::categorize()
{
   categorizeVert();
   categorizeHoriz();
   m_log.s().s() << "center appears to be at (" << m_center.x << "," << m_center.y << ")" << std::endl;
}

void whiskerSurvey::categorizeVert()
{
   long w,h;
   m_canvas.getDims(w,h);

   // vert whiskers _can_ be on the extreme left but not extreme right
   // ... meh, who cares
   bool done = false;
   for(long y=0;y<h&&!done;y++)
   {
      COLORREF last;
      for(long x=0;x<w&&!done;x++)
      {
         COLORREF pix = m_canvas.getPixel(point(x,y));
         if(x)
         {
            bool twoInARow = (pix == last);
            if(twoInARow && pix != RGB(255,255,255))
            {
               done = true; // no longer looking at whiskers!
               m_center.y = ((h-y) / 2) + y;
            }
            else if(last != RGB(255,255,255))
            {
               // consider last a whisker
               markVertWhisker(x-1,y,last);
            }
         }
         last = pix;
      }
   }
}

void whiskerSurvey::categorizeHoriz()
{
   long w,h;
   m_canvas.getDims(w,h);

   // vert whiskers _can_ be on the extreme left but not extreme right
   // ... meh, who cares
   bool done = false;
   for(long x=0;x<w&&!done;x++)
   {
      COLORREF last;
      for(long y=0;y<h&&!done;y++)
      {
         COLORREF pix = m_canvas.getPixel(point(x,y));
         if(y)
         {
            bool twoInARow = (pix == last);
            if(twoInARow && pix != RGB(255,255,255))
            {
               done = true; // no longer looking at whiskers!
               m_center.x = ((w-x) / 2) + x;
            }
            else if(last != RGB(255,255,255))
            {
               // consider last a whisker
               markHorizWhisker(x,y-1,last);
            }
         }
         last = pix;
      }
   }
}

void whiskerSurvey::markVertWhisker(long x, long y, COLORREF c)
{
   bool noob = (m_vertWhiskers.find(c) == m_vertWhiskers.end());
   if(!noob && m_vertWhiskers[c] != x)
      throw std::runtime_error("two whiskers with the same color?");
   else
      m_vertWhiskers[c] = x;
   m_whiskerPoints.push_back(point(x,y));
}

void whiskerSurvey::markHorizWhisker(long x, long y, COLORREF c)
{
   bool noob = (m_horizWhiskers.find(c) == m_horizWhiskers.end());
   if(!noob && m_horizWhiskers[c] != y)
      throw std::runtime_error("two whiskers with the same color?");
   else
      m_horizWhiskers[c] = y;
   m_whiskerPoints.push_back(point(x,y));
}
