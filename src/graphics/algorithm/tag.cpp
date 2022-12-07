#include "../../crayon/log.hpp"
#include "../algorithm.hpp"

// tags are encoded in 100px wide blocks, of arbitrary depth
// they are always encased in purple, which is the reserved color meaning nothing
// each pixel encodes three letters/bytes
const COLORREF tagWriter::tagBgCol = RGB(200,191,231); // light purple

long tagWriter::write(const std::string& text)
{
   // write the message, encoded in the the B channel
   point pt(2,2);
   const char *pThumb = text.c_str();
   for(size_t i=0;i<text.length()+1;i++,pThumb++)
   {
      m_c.setPixel(pt,RGB(30,100,*pThumb));
      pt.x++;
      if(pt.x == 100)
      {
         pt.x = 2;
         pt.y++;
      }
   }

   // fill in the rest of the bottom line
   for(;pt.x != 100;pt.x++)
      m_c.setPixel(pt,tagBgCol);

   // outline the tag in a black box
   // top/bottom
   for(long x=0;x<102;x++)
   {
      m_c.setPixel(point(x,0),RGB(0,0,0));
      m_c.setPixel(point(x,1),tagBgCol);
      m_c.setPixel(point(x,pt.y+1),tagBgCol);
      m_c.setPixel(point(x,pt.y+2),RGB(0,0,0));
   }
   // left/right
   for(long y=0;y<pt.y+2;y++)
   {
      m_c.setPixel(point(0,y),RGB(0,0,0));
      m_c.setPixel(point(101,y),RGB(0,0,0));
   }
   // left/right (purple)
   for(long y=1;y<pt.y+1;y++)
   {
      m_c.setPixel(point(1,y),tagBgCol);
      m_c.setPixel(point(100,y),tagBgCol);
   }

   return pt.y;
}

std::string tagReader::readIf()
{
   std::stringstream stream;

   point pt(2,2);
   while(true)
   {
      COLORREF pix = m_c.getPixel(pt);

      bool isValid;
      char c = getTagChar(pix,isValid);
      if(!isValid)
         return "";

      if(c == 0)
         return stream.str();
      stream << std::string(1,c);

      pt.x++;
      if(pt.x == 100)
      {
         pt.x = 2;
         pt.y++;
      }
   }
}

char tagReader::getTagChar(COLORREF c, bool& isValid)
{
   isValid = true;
   if(GetRValue(c) != 30)
      isValid = false; // not a tag
   if(GetGValue(c) != 100)
      isValid = false; // not a tag
   return (char)GetBValue(c);
}
