#include "../crayon/log.hpp"
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

void objectFinder::run(iCanvas& c, log& l)
{
   objectFinder self(c,l);
   self._run();
}

objectFinder::objectFinder(iCanvas& c, log& l)
: m_canvas(c)
, m_log(l)
, m_nextObjId(1)
{
   m_canvas.getDims(m_w,m_h);
}

void objectFinder::_run()
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

size_t objectFinder::findAdjacentMembership(const point& p)
{
   size_t ans = 0;

   if(p.x)
   {
      auto it = m_map.find(point(p.x-1,p.y));
      if(it!=m_map.end())
         ans = mergeObjectsIf(ans,it->second);
   }
   if(p.y)
   {
      auto it = m_map.find(point(p.x,p.y-1));
      if(it!=m_map.end())
         ans = mergeObjectsIf(ans,it->second);
   }
   if(p.x && p.y)
   {
      auto it = m_map.find(point(p.x-1,p.y-1));
      if(it!=m_map.end())
         ans = mergeObjectsIf(ans,it->second);
   }
   if(p.y)
   {
      auto it = m_map.find(point(p.x-1,p.y+1));
      if(it!=m_map.end())
         ans = mergeObjectsIf(ans,it->second);
   }
   return ans;
}

void objectFinder::addToObject(const point& p, size_t i)
{
   if(i == 0)
   {
      m_log.s().s() << "[find-object] found new object " << i << " starting at (" << p.x << "," << p.y << ")" << std::endl;
      i = m_nextObjId++;
   }
   m_objects[i].insert(p);
}

size_t objectFinder::mergeObjectsIf(size_t oldObj, size_t newObj)
{
   if(oldObj == 0)
      return newObj;

   m_log.s().s() << "[find-object] merging objects " << oldObj << " <- " << newObj << std::endl;

   // move all newObj's pnts to oldObj
   std::set<point>& newPnts = m_objects[newObj];
   for(auto p : newPnts)
   {
      m_map[p] = oldObj;
      m_objects[oldObj].insert(p);
   }
   m_objects.erase(newObj);
   return oldObj;
}
