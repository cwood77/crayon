#include "graphicsApi.hpp"
#include <cmath>

#include <cstdio>

polPoint polPoint::fromCartesian(const point& p)
{
   polPoint pp;

   pp.r = sqrt(p.x*p.x + p.y*p.y);
   pp.theta = ::atan2(p.y,p.x);

   return pp;
}

point polPoint::toCartesian() const // (420,417)
{
   point cp(0,0);
   cp.x = ::round(r * ::cos(theta));
   cp.y = ::round(r * ::sin(theta));
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

colorInfo& colorInfo::fromRgb(COLORREF c)
{
   // all formulae taken from https://www.rapidtables.com/convert/color/rgb-to-hsv.html

   double r = GetRValue(c) / 255.0;
   double g = GetGValue(c) / 255.0;
   double b = GetBValue(c) / 255.0;

   auto Cmax = r > g ? r : g;
   Cmax = Cmax > b ? Cmax : b;
   auto Cmin = r < g ? r : g;
   Cmin = Cmin < b ? Cmin : b;
   auto delta = Cmax - Cmin;

   // HSV (paintbrush uses this)
   if(delta == 0.0)
      hue = 0;
   else if(Cmax == r)
      hue = 60 * ::fmod((g-b)/delta,6);
   else if(Cmax == g)
      hue = 60 * ((b-r)/delta + 2);
   else if(Cmax == b)
      hue = 60 * ((r-g)/delta + 4);
   if(Cmax == 0)
      sat = 0;
   else
      sat = delta / Cmax;
   val = Cmax;

   // HSI
   lightness = (Cmax + Cmin) / 2.0;

   return *this;
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

void drawBox(const RECT& r, COLORREF col, iCanvas& can)
{
   for(long x=r.left;x<=r.right;x++)
   {
      can.setPixel(point(x,r.top),col);
      can.setPixel(point(x,r.bottom),col);
   }
   for(long y=r.top;y<=r.bottom;y++)
   {
      can.setPixel(point(r.left,y),col);
      can.setPixel(point(r.right,y),col);
   }
}

void drawBox(const RECT& r, COLORREF col, COLORREF fillCol, iCanvas& can)
{
   for(long x=r.left;x<r.right;x++)
      for(long y=r.top;y<r.bottom;y++)
         can.setPixel(point(x,y),fillCol);

   drawBox(r,col,can);
}
