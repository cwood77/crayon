#include "../../crayon/log.hpp"
#include "../algorithm.hpp"

objectSurvey::objectSurvey(iCanvas& c, log& l)
: m_canvas(c)
, m_log(l)
, m_nextObjId(1)
, m_consumeTags(false)
{
   m_canvas.getDims(m_w,m_h);
   run();
}

rect& objectSurvey::findObject(size_t n)
{
   if(n >= m_objects.size())
      throw std::runtime_error("object index out of bounds");

   auto it = m_bounds.find(n);
   if(it==m_bounds.end())
   {
      // lazy-create the bounds

      // objects get created and destroyed/merged during find, so
      // object IDs do not strictly correlate to n (i.e. bounds indicies)
      auto oit = m_objects.begin();
      for(size_t i=0;i<n;i++,++oit);

      makeBounds(n,oit->first);
      storeAndTrimTagIf(n,oit->first);

      it = m_bounds.find(n);
   }

   return it->second;
}

rect& objectSurvey::findObjectByTag(const std::string& tag)
{
   // let findObject do all the work
   for(size_t i=0;i<getNumFoundObjects();i++)
      if(getTag(i) == tag)
         return findObject(i);
   throw std::runtime_error("tag not found: " + tag);
}

std::string objectSurvey::getTag(size_t n)
{
   auto it = m_tags.find(n);
   if(it == m_tags.end())
   {
      // let findObject do all the work
      findObject(n);
      it = m_tags.find(n);
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

// calculate bounds of this object by considering each member
// point, and skipping the tagHeight
void objectSurvey::makeBounds(size_t bndsIdx, size_t objId)
{
   long th = 0;
   auto it = m_objectTagHeight.find(objId);
   bool hasTh = (it!=m_objectTagHeight.end());
   if(hasTh)
      th = it->second;

   auto& pts = m_objects[objId];
   for(auto pt : pts)
      if(!hasTh || pt.y > th)
         makeBounds(bndsIdx,pt);
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

void objectSurvey::storeAndTrimTagIf(size_t bndsIdx, size_t objId)
{
   rect r = m_bounds[bndsIdx];
   autoReleasePtr<iCanvas> pSub(m_canvas.subset(r));
   auto tag = tagReader(pSub,m_log).readIf();
   if(!tag.empty() && m_consumeTags)
   {
      // remove the tag from the object
      size_t tagWidth = countPixels(pSub,0,r.w);
      size_t tagHeight = 1;
      long y=1;
      for(;y<r.h && countPixels(pSub,y,r.w)==tagWidth;y++) tagHeight++;
      size_t lineHeight = 0;
      for(;y<r.h && countPixels(pSub,y,r.w)==1;y++) lineHeight++;

      // record height to omit, and recalcuate the bounds
      m_objectTagHeight[objId] = r.y + tagHeight + lineHeight - 1;
      m_bounds.erase(bndsIdx);
      makeBounds(bndsIdx,objId);
   }

   m_tags[bndsIdx] = tag;
}

size_t objectSurvey::countPixels(iCanvas& c, long y, long w)
{
   size_t n = 0;
   for(long x=0;x<w;x++)
   {
      auto col = c.getPixel(point(x,y));
      if(col != RGB(255,255,255))
         n++;
   }
   return n;
}
