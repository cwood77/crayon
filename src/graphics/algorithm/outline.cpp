#include "../../crayon/log.hpp"
#include "../algorithm.hpp"

bool lightnessPixelCriteria::isEligible(COLORREF c)
{
   return m_minLightness < colorInfo().fromRgb(c).lightness;
}

void outliner::encroach(iPixelCriteria& c, COLORREF col)
{
   std::set<point> encroached;

   while(true)
   {
      bool madeProgress = false;

      std::set<point> frameEdge,offEdge;
      m_f.calculateOutline(frameEdge,offEdge);
      for(auto pt : offEdge)
      {
         if(c.isEligible(m_c.getPixel(pt)))
         {
            // claim this sucka
            m_f.markIn(pt);
            m_c.setPixel(pt,m_f.getFrameColor());
            madeProgress = true;
            encroached.insert(pt);
         }
      }

      if(!madeProgress)
         break; // no longer finding dudes
   }

   m_log.s().s() << "outliner encroached " << encroached.size() << " 'close-enough' pixels" << std::endl;

   for(auto pt : encroached)
      m_c.setPixel(pt,col);
}

void outliner::retreat(COLORREF c)
{
   std::set<point> frameEdge,offEdge;
   m_f.calculateOutline(frameEdge,offEdge);
   for(auto pt : frameEdge)
   {
      m_f.unmark(pt);
      m_c.setPixel(pt,c);
   }

   m_log.s().s() << "outliner retreated " << frameEdge.size() << " pixels" << std::endl;
}
