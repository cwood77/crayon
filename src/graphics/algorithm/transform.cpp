#include "../../crayon/log.hpp"
#include "../algorithm.hpp"

COLORREF componentShift::run(COLORREF c)
{
   long v = 0;
   long a = 0;
   long b = 0;

   if(m_c == 'r')
   {
      v = GetRValue(c);
      a = GetGValue(c);
      b = GetBValue(c);
   }
   else if(m_c == 'g')
   {
      a = GetRValue(c);
      v = GetGValue(c);
      b = GetBValue(c);
   }
   else if(m_c == 'b')
   {
      a = GetRValue(c);
      b = GetGValue(c);
      v = GetBValue(c);
   }
   else
      throw std::runtime_error("unknown component");

   v += m_s;
   if(v > 255)
   {
      // maxed out, ceil it, then compensate the others
      long over = v - 255;
      v = 255;
      a = diffMin0(a,over);
      b = diffMin0(b,over);
   }
   else if(v < 0)
   {
      // mined out, floor it, then compensate the others
      long under = 0 - v;
      v = 0;
      a = addMax255(a,under);
      b = addMax255(b,under);
   }

   if(m_c == 'r')
      return RGB(v,a,b);
   else if(m_c == 'g')
      return RGB(a,v,b);
   else
      return RGB(a,b,v);
}

COLORREF lightnessShift::run(COLORREF c)
{
   return RGB(
      adjByteBndChk(GetRValue(c),m_s),
      adjByteBndChk(GetGValue(c),m_s),
      adjByteBndChk(GetBValue(c),m_s));
}

COLORREF toMonochromeShift::run(COLORREF c)
{
   if(m_c.isEligible(c))
      return RGB(255,255,255); // white
   else
      return RGB(0,0,0); // black
}

void pixelTransformer::run(iPixelTransform& t)
{
   long w,h;
   m_c.getDims(w,h);
   for(long y=0;y<h;y++)
   {
      for(long x=0;x<w;x++)
      {
         auto p = m_c.getPixel(point(x,y));
         auto p2 = t.run(p);
         if(p != p2)
            m_c.setPixel(point(x,y),p2);
      }
   }
}
