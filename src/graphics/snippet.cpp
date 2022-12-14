#include "snippet.hpp"
#include <cmath>
#include <stdexcept>

snippet::snippet(long w, long h)
: m_w(w), m_h(h)
{
   m_pBlock = new pixel [w*h];
   ::memset(m_pBlock,0xFF,sizeof(pixel)*w*h);
}

snippet::~snippet()
{
   delete [] m_pBlock;
}

pixel& snippet::index(const point& p)
{
   if(p.x < 0 || p.y < 0)
   {
      ::printf("point was (%ld,%ld)\n",p.x,p.y);
      throw std::runtime_error("point out of bounds 0");
   }
   if(p.x >= m_w || p.y >= m_h)
      throw std::runtime_error("point out of bounds 1");
   return m_pBlock[p.x + p.y*m_w];
}

rotateTransform::rotateTransform(double angle)
: m_offset(0,0)
{
   // convert from degrees to radians
   m_angle = angle * M_PI / 180.0;
}

void rotateTransform::translateDims(long& w, long& h)
{
   long max = w > h ? w : h;

   w = max + max;
   h = max + max;

   m_offset.x = max;
   m_offset.y = max;

   ::printf("allocating size for (%ld,%ld) bytes\n",w,h);
   ::printf("h offset is %ld\n",m_offset.y);
}

void rotateTransform::translateCoords(point& p)
{
   point cartPnt = p;
   toCartesian(cartPnt);
   auto polar = polPoint::fromCartesian(cartPnt);
   // pi/2 = 90 degrees
   // subtract means rotate clockwise
   polar.theta += m_angle;
   cartPnt = polar.toCartesian();
   p = cartPnt;
   toGraphics(p);
}

void rotateTransform::toCartesian(point& p)
{
   p = point(p.x,-p.y);
}

void rotateTransform::toGraphics(point& p)
{
   p = point(p.x+m_offset.x,-p.y+m_offset.y);
}
