#include "graphicsApi.hpp"
#include <cmath>

#include <cstdio>

polPoint polPoint::fromCartesian(const point& p)
{
   polPoint pp;

   pp.r = sqrt(p.x*p.x + p.y*p.y);
   pp.theta = ::atan2(p.y,p.x);

   ::printf("XY(%ld,%ld) => rT(%lf,%lf)\n",p.x,p.y,pp.r,pp.theta);
   return pp;
}

point polPoint::toCartesian() const // (420,417)
{
   point cp(0,0);
   cp.x = r * ::cos(theta);
   cp.y = r * ::sin(theta);
   ::printf("rT(%lf,%lf) => XY(%ld,%ld)\n",r,theta,cp.x,cp.y);
   ::printf("      ...(cos(t)=%lf)\n",::cos(theta));
   ::printf("      ...(r=%lf)\n",r);
   ::printf("      ...(r'=%lf)\n",r*::cos(theta));
   ::printf("      ...(r'=%ld)\n",(long)(r*::cos(theta)));
   ::printf("      ...(r''=%ld)\n",(long)(-1.0));
   double fuckingNeg1 = -1.0;
   ::printf("      ...(r'''=%ld)\n",(long)fuckingNeg1);
   return cp;
}

void rect::growToInclude(const point& p)
{
   // X
   if(p.x < x)
   {
      w += (x-p.x);
      x = p.x;
   }
   else if(p.x > x+w-1)
      w += (p.x - (x+w-1));

   // Y
   if(p.y < y)
   {
      h += (y-p.y);
      y = p.y;
   }
   else if(p.y > y+h-1)
      h += (p.y - (y+h-1));
}

RECT rect::toRect() const
{
   RECT r;
   r.left = x;
   r.top = y;
   r.right = x + w - 1;
   r.bottom = y + h - 1;
   return r;
}

BYTE adjByteBndChk(BYTE b, long d)
{
   long x = (long)b + d;
   if(x > 255)
      return 255;
   else if(x < 0)
      return 0;
   else
      return x;
}
