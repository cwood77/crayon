#include "../../crayon/log.hpp"
#include "../../crayon/symbolTable.hpp"
#include "../../frontend/eval.hpp"
#include "../algorithm.hpp"

iSymbol& unattestedColorFinder::complete()
{
   BYTE o[3] = { 0, 0, 0 };

   for(size_t i=0;;i++)
   {
      if(m_attested.find(m_col)==m_attested.end())
         break;

      size_t comp = i % 3;

      if(o[comp] < 255)
         o[comp]++;
      else if(comp == 2)
         throw std::runtime_error("all colors appear to be attested!");

      m_col = RGB(
         GetRValue(m_col) - o[0],
         GetGValue(m_col) - o[1],
         GetBValue(m_col) - o[2]
      );
   }

   return *new stringSymbol(argEvaluator::fmtColor(m_col));
}

void pixelAnalyzer::run(iPixelAnalysis& a, const std::string& varName)
{
   long w,h;
   m_c.getDims(w,h);
   for(long y=0;y<h;y++)
   {
      for(long x=0;x<w;x++)
      {
         auto p = m_c.getPixel(point(x,y));
         a.addPixel(p);
      }
   }

   a.complete();
   m_sTable.overwrite(varName,a.complete());
}
