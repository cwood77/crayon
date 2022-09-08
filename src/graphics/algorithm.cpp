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

rect objectFinder::run(iCanvas& c, size_t n, bool dbgHilight, log& l)
{
   objectFinder self(c,l);
   return self._run(n,dbgHilight);
}

objectFinder::objectFinder(iCanvas& c, log& l)
: m_canvas(c)
, m_log(l)
, m_nextObjId(1)
{
   m_canvas.getDims(m_w,m_h);
}

rect objectFinder::_run(size_t n, bool dbgHilight)
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

   m_log.s().s() << "found " << m_objects.size() << " object(s)" << std::endl;
   for(auto it=m_objects.begin();it!=m_objects.end();++it)
   {
      for(auto pt : it->second)
         makeBounds(it->first,pt);

      auto& r = m_bounds[it->first];
      m_log.s().s() << "object " << it->first << " {"
         << r.x << ","
         << r.y << ","
         << r.w << ","
         << r.h << "}" << std::endl;

      if(dbgHilight)
         hilight(r,RGB(0,255,0));
   }

   if(n >= m_bounds.size())
      throw std::runtime_error("object index out of bounds");
   else
   {
      auto it = m_bounds.begin();
      for(size_t i=0;i<n;i++)
         ++it;
      return it->second;
   }
}

// b/c of the direction of the sweep, it's only worth looking above and to the left
// for precedent
size_t objectFinder::findAdjacentMembership(const point& p)
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

void objectFinder::addToObject(const point& p, size_t i)
{
   if(i == 0)
   {
      i = m_nextObjId++;
      m_log.s().s() << "[find-object] found new object " << i << " starting at (" << p.x << "," << p.y << ")" << std::endl;
   }
   m_objects[i].insert(p);
   m_map[p] = i;
}

size_t objectFinder::mergeObjectsIf(size_t oldObj, size_t newObj)
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

   m_log.s().s() << "[find-object] merging objects " << winer << " <- " << loser << std::endl;

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

void objectFinder::makeBounds(size_t id, const point& p)
{
   auto it = m_bounds.find(id);
   bool noob = (it == m_bounds.end());
   rect& r = m_bounds[id];
   if(noob)
      r.setOrigin(p);
   else
      r.growToInclude(p);
}

void objectFinder::hilight(const rect& r, COLORREF c)
{
   // h lines
   for(long x=r.x;x<r.x+r.w;x++)
   {
      m_canvas.setPixel(point(x,r.y),c);
      m_canvas.setPixel(point(x,r.y+r.h-1),c);
   }

   // vlines
   for(long y=r.y;y<r.y+r.h;y++)
   {
      m_canvas.setPixel(point(r.x,y),c);
      m_canvas.setPixel(point(r.x+r.w-1,y),c);
   }
}

point whiskerFinder::run(iCanvas& c, COLORREF x, COLORREF y, log& Log)
{
   whiskerFinder self(c,Log);
   self.categorize();
   return self.find(x,y);
}

whiskerFinder::whiskerFinder(iCanvas& c, log& l)
: m_canvas(c), m_log(l), m_clearWhiskers(false)
{
}

void whiskerFinder::categorizeVert()
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
               done = true; // no longer looking at whiskers!
            else if(last != RGB(255,255,255))
            {
               // consider last a whisker
               markVertWhisker(x-1,y,last);

               if(m_clearWhiskers)
                  m_canvas.setPixel(point(x-1,y),RGB(255,255,255));
            }
         }
         last = pix;
      }
   }
}

void whiskerFinder::categorizeHoriz()
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
               done = true; // no longer looking at whiskers!
            else if(last != RGB(255,255,255))
            {
               // consider last a whisker
               markHorizWhisker(x-1,y,last);

               if(m_clearWhiskers)
                  m_canvas.setPixel(point(x,y-1),RGB(255,255,255));
            }
         }
         last = pix;
      }
   }
}

void whiskerFinder::markVertWhisker(long x, long y, COLORREF c)
{
   bool noob = (m_vertWhiskers.find(c) == m_vertWhiskers.end());
   if(!noob && m_vertWhiskers[c] != x)
      throw std::runtime_error("two whiskers with the same color?");
   else
      m_vertWhiskers[c] = x;
}

void whiskerFinder::markHorizWhisker(long x, long y, COLORREF c)
{
   bool noob = (m_horizWhiskers.find(c) == m_horizWhiskers.end());
   if(!noob && m_horizWhiskers[c] != y)
      throw std::runtime_error("two whiskers with the same color?");
   else
      m_horizWhiskers[c] = y;
}

point whiskerFinder::find(COLORREF x, COLORREF y)
{
   m_log.s().s() << "find (" << x << "," << y << ")" << std::endl;

   auto vit = m_vertWhiskers.find(x);
   auto hit = m_horizWhiskers.find(y);

   if(hit == m_horizWhiskers.end() || vit == m_vertWhiskers.end())
      throw std::runtime_error("whisker not found");

   //return point(vit->second,hit->second-1); // WTF?
   return point(vit->second,hit->second); // WTF?
}
