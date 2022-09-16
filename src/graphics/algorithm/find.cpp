#include "../../crayon/log.hpp"
#include "../algorithm.hpp"

objectSurvey::objectSurvey(iCanvas& c, log& l)
: m_canvas(c)
, m_log(l)
, m_nextObjId(1)
{
   m_canvas.getDims(m_w,m_h);
   run();
}

rect objectSurvey::findObject(size_t n)
{
   if(n >= m_objects.size())
      throw std::runtime_error("object index out of bounds");

   auto it = m_bounds.find(n);
   if(it==m_bounds.end())
   {
      // lazy-create the bounds

      // objects get created and destroyed/merged during find, so
      // object IDs do not strictly correlate to n
      auto oit = m_objects.begin();
      for(size_t i=0;i<n;i++,++oit);

      auto& pts = oit->second;
      for(auto pt : pts)
         makeBounds(n,pt);
      it = m_bounds.find(n);
   }

   return it->second;
}

void objectSurvey::run()
{
   for(long y=0;y<m_h;y++)
   {
      for(long x=0;x<m_w;x++)
      {
         auto col = m_canvas.getPixel(point(x,y));
         if(col == RGB(255,255,255))
            continue;

         auto objId = findAdjacentMembership(point(x,y));
         addToObject(point(x,y),objId);
      }
   }
}

// b/c of the direction of the sweep, it's only worth looking above and to the left
// for precedent
size_t objectSurvey::findAdjacentMembership(const point& p)
{
   size_t ans = 0;

   if(p.x) // left
   {
      auto it = m_map.find(point(p.x-1,p.y));
      if(it!=m_map.end())
         ans = mergeObjectsIf(ans,it->second);
   }
   if(p.y) // above
   {
      auto it = m_map.find(point(p.x,p.y-1));
      if(it!=m_map.end())
         ans = mergeObjectsIf(ans,it->second);
   }
   if(p.x && p.y) // catacorner above-left
   {
      auto it = m_map.find(point(p.x-1,p.y-1));
      if(it!=m_map.end())
         ans = mergeObjectsIf(ans,it->second);
   }
   if(p.y) // catacorner above-right
   {
      auto it = m_map.find(point(p.x+1,p.y-1));
      if(it!=m_map.end())
         ans = mergeObjectsIf(ans,it->second);
   }
   return ans;
}

void objectSurvey::addToObject(const point& p, size_t i)
{
   if(i == 0)
      i = m_nextObjId++;
   m_objects[i].insert(p);
   m_map[p] = i;
}

size_t objectSurvey::mergeObjectsIf(size_t oldObj, size_t newObj)
{
   // winer is the smaller of the two
   size_t loser = oldObj;
   size_t winer = newObj;
   if(winer > loser)
   {
      loser = newObj;
      winer = oldObj;
   }

   if(winer == 0 || loser == winer)
      return loser;

   // move all newObj's pnts to oldObj
   std::set<point>& lPnts = m_objects[loser];
   for(auto p : lPnts)
   {
      m_map[p] = winer;
      m_objects[winer].insert(p);
   }
   m_objects.erase(loser);
   return winer;
}

void objectSurvey::makeBounds(size_t id, const point& p)
{
   auto it = m_bounds.find(id);
   bool noob = (it == m_bounds.end());
   rect& r = m_bounds[id];
   if(noob)
      r.setOrigin(p);
   else
      r.growToInclude(p);
}
