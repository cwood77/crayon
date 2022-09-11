#include "graphicsApi.hpp"

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
