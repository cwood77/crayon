#define WIN32_LEAN_AND_MEAN
#include "../crayon/symbolTable.hpp"
#include "../crayon/test.hpp"
#include "eval.hpp"
#include <cstring>
#include <list>
#include <windows.h>

namespace {

void expandInterpolationParts(const std::string& in, std::list<std::string>& parts)
{
   const char *pThumb = in.c_str();
   const char *pStart = pThumb;
   size_t braceDepth = 0;
   for(;true;++pThumb)
   {
      if(::strncmp(pThumb,"{$",2)==0)
         braceDepth++;

      if(*pThumb == '$')
      {
         if(braceDepth && pThumb-pStart-1)
            parts.push_back(std::string(pStart,pThumb-pStart-1));
         else if(!braceDepth && pStart!=pThumb)
            parts.push_back(std::string(pStart,pThumb-pStart));
         pStart = pThumb;
      }
      else if(*pThumb == '}' && braceDepth)
      {
         if(pStart != pThumb)
            parts.push_back(std::string(pStart,pThumb-pStart));
         braceDepth--;
         pStart = pThumb + 1;
      }
      else if(*pThumb == 0)
      {
         if(braceDepth)
            throw std::runtime_error("unterminated brace");
         if(pStart != pThumb)
            parts.push_back(std::string(pStart,pThumb-pStart));
         break;
      }
   }
}

} // anonymous namespace

size_t argEvaluator::computeBitFlags(iSymbolTable& st, const std::list<std::string>& options, std::map<std::string,size_t>& table)
{
   size_t ans = 0;
   for(auto opt : options)
      ans |= argEvaluator(st,opt).lookup(table);
   return ans;
}

std::string argEvaluator::fmtColor(size_t c)
{
   std::stringstream value;
   value
      << "rgb{"
      << (int)GetRValue(c) << ","
      << (int)GetGValue(c) << ","
      << (int)GetBValue(c) << "}"
   ;
   return value.str();
}

std::string argEvaluator::fmtPoint(const point& p)
{
   std::stringstream value;
   value
      << "pnt{" << p.x << "," << p.y << "}"
   ;
   return value.str();
}

std::string argEvaluator::fmtRect(const rect& r)
{
   std::stringstream value;
   value
      << "rect[tl,br]{pnt{" << r.x << "," << r.y << "},"
      << "pnt{" << (r.x+r.w-1) << "," << (r.y+r.h-1) << "}"
      << "}"
   ;
   return value.str();
}

// expand variables
// expansion forms:
//    simple:       $g[0].f
//    interpolated: thing{$g[0].f}
std::string argEvaluator::getString()
{
   std::list<std::string> parts;
   expandInterpolationParts(m_in,parts);
   std::stringstream out;
   for(auto part : parts)
   {
      if(part.length() > 1 && part.c_str()[0] == '$')
         out << m_sTable.demand(part.c_str()).asString();
      else
         out << part;
   }
   return out.str();
}

long argEvaluator::getInt()
{
   std::string in = getString();
   long x;
   int rval = ::sscanf(in.c_str(),"%ld",&x);
   if(rval != 1)
      throw std::runtime_error("failed to parse int arg");
   return x;
}

double argEvaluator::getReal()
{
   std::string in = getString();
   double x;
   int rval = ::sscanf(in.c_str(),"%lf",&x);
   if(rval != 1)
      throw std::runtime_error("failed to parse real arg");
   return x;
}

bool argEvaluator::getFlag(const std::string& name)
{
   std::string in = getString();
   return in == name;
}

size_t argEvaluator::lookup(std::map<std::string,size_t>& table)
{
   std::string in = getString();
   auto it = table.find(in);
   if(it == table.end())
      throw std::runtime_error("arg value not in table");
   return it->second;
}

std::list<std::string> argEvaluator::getSet()
{
   std::string in = getString();

   std::list<std::string> set;
   const char *pThumb = in.c_str();
   if(*pThumb != '{') throw std::runtime_error("syntax error in set");
   ++pThumb;
   const char *pStart = pThumb;
   while(true)
   {
      for(;*pThumb!=0&&*pThumb!=','&&*pThumb!='}';++pThumb); // eat until comma or }
      if(*pThumb=='}')
      {
         set.push_back(std::string(pStart,pThumb-pStart));
         break;
      }
      else if(*pThumb==0)
         throw std::runtime_error("unterminated set");
      else if(*pThumb==',')
      {
         set.push_back(std::string(pStart,pThumb-pStart));
         ++pThumb;
         pStart = pThumb;
      }
   }
   return set;
}

size_t argEvaluator::getColor()
{
   std::string in = getString();

   if(in == "nocol")
      return 0xFFFFFFFF;

   if(::strncmp(in.c_str(),"rgb{",4)!=0)
      throw std::runtime_error("invalid color syntax");
   unsigned long r,g,b;
   auto rval = ::sscanf(in.c_str()+4,"%lu,%lu,%lu",&r,&g,&b);
   if(rval != 3) throw std::runtime_error("can't parse color");
   return RGB(r,g,b);
}

point argEvaluator::getPoint()
{
   std::string in = getString();

   if(::strncmp(in.c_str(),"pnt[center:tl,dims]{pnt{",24)==0)
   {
      unsigned long x,y;
      rect r;
      parsePointAndRect(in.c_str()+24,x,y,r);

      return point(
         x + (r.w / 2),
         y + (r.h / 2)
      );
   }

   if(::strncmp(in.c_str(),"pnt[tl:center,dims]{pnt{",24)==0)
   {
      unsigned long x,y;
      rect r;
      parsePointAndRect(in.c_str()+24,x,y,r);

      return point(
         x - (r.w / 2),
         y - (r.h / 2)
      );
   }

   if(::strncmp(in.c_str(),"pnt{",4)!=0)
      throw std::runtime_error("invalid point syntax");
   unsigned long x,y;
   auto rval = ::sscanf(in.c_str()+4,"%lu,%lu",&x,&y);
   if(rval != 2) throw std::runtime_error("can't parse point");
   return point(x,y);
}

rect argEvaluator::getRect()
{
   std::string in = getString();

   if(::strncmp(in.c_str(),"rect[tl,br]{pnt{",16)==0)
   {
      unsigned long x,y,x2,y2;
      auto rval = ::sscanf(in.c_str()+16,"%lu,%lu},pnt{%lu,%lu",&x,&y,&x2,&y2);
      if(rval != 4) throw std::runtime_error("can't parse rect");
      return rect(x,y,x2-x+1,y2-y+1);
   }
   else if(::strncmp(in.c_str(),"rect[l,r,b]{pnt{",16)==0) // used?
   {
      unsigned long lx,ly,rx,ry,bx,by;
      auto rval = ::sscanf(in.c_str()+16,"%lu,%lu},pnt{%lu,%lu},pnt{%lu,%lu",
         &lx,&ly,&rx,&ry,&bx,&by);
      if(rval != 6) throw std::runtime_error("can't parse rect");
      const size_t hugeSize = 100000;
      return rect(
         lx,
         by-(hugeSize-1),
         (rx-lx)+1,
         hugeSize);
   }
   else
      throw std::runtime_error("invalid rect syntax");
}

colorRange argEvaluator::getColorRange()
{
   std::string in = getString();

   if(::strncmp(in.c_str(),"colrange[rgb+/-hsv]{rgb{",24)==0)
   {
      unsigned long r,g,b;
      double h,s,v;
      auto rval = ::sscanf(in.c_str()+24,"%lu,%lu,%lu}+/-hsv{%lf,%lf,%lf}}",
         &r,&g,&b,&h,&s,&v);
      if(rval != 6) throw std::runtime_error("can't parse color range");

      colorRange cr;
      cr.base.fromRgb(RGB(r,g,b));
      cr.hDelta = h;
      cr.sDelta = s;
      cr.vDelta = v;
      return cr;
   }
   else
      throw std::runtime_error("invalid color range syntax");
}

void argEvaluator::getFont(std::string& face, size_t& pnt)
{
   std::string in = getString();

   if(::strncmp(in.c_str(),"font{'",6)!=0)
      throw std::runtime_error("invalid font syntax");
   char buffer[MAX_PATH];
   auto rval = ::sscanf(in.c_str()+6,"%[^']',%llu",buffer,&pnt);
   if(rval != 2) throw std::runtime_error("can't parse font");
   face = buffer;
}

size_t argEvaluator::getPixelCount()
{
   std::string in = getString();

   double x;
   char units[MAX_PATH];
   int rval = ::sscanf(in.c_str(),"%lf%s",&x,units);
   if(rval != 2)
      throw std::runtime_error("failed to parse measurment");
   if(std::string(units) != "px")
      throw std::runtime_error("unknown units: " + std::string(units));
   return (size_t)x;
}

void argEvaluator::parsePointAndRect(const std::string& text, unsigned long& x, unsigned long& y, rect& r)
{
   int n = 0;
   auto rval = ::sscanf(text.c_str(),"%lu,%lu%n",&x,&y,&n);
   if(rval != 2 || !n) throw std::runtime_error("can't parse point (191)");
   if(::strncmp(text.c_str()+n,"},",2)!=0)
      throw std::runtime_error("invalid point syntax (193)");

   r = argEvaluator(m_sTable,text.c_str()+n+2).getRect();
}

#ifdef cdwTestBuild

cdwTest(interpolation_empty)
{
   std::list<std::string> parts;
   expandInterpolationParts("",parts);

   std::stringstream expected,actual;
   for(auto s : parts)
      actual << s << std::endl;

   cdwAssertEqu(expected.str(),actual.str());
}

cdwTest(interpolation_plaintext)
{
   std::list<std::string> parts;
   expandInterpolationParts("hello world",parts);

   std::stringstream expected,actual;
   expected
      << "hello world" << std::endl
   ;
   for(auto s : parts)
      actual << s << std::endl;

   cdwAssertEqu(expected.str(),actual.str());
}

cdwTest(interpolation_oneRef)
{
   std::list<std::string> parts;
   expandInterpolationParts("$g[0].bar",parts);

   std::stringstream expected,actual;
   expected
      << "$g[0].bar" << std::endl
   ;
   for(auto s : parts)
      actual << s << std::endl;

   cdwAssertEqu(expected.str(),actual.str());
}

cdwTest(interpolation_oneRefBrace)
{
   std::list<std::string> parts;
   expandInterpolationParts("{$g[0].bar}",parts);

   std::stringstream expected,actual;
   expected
      << "$g[0].bar" << std::endl
   ;
   for(auto s : parts)
      actual << s << std::endl;

   cdwAssertEqu(expected.str(),actual.str());
}

cdwTest(interpolation_fancy)
{
   std::list<std::string> parts;
   expandInterpolationParts("font{'{$typeface}',10}",parts);

   std::stringstream expected,actual;
   expected
      << "font{'" << std::endl
      << "$typeface" << std::endl
      << "',10}" << std::endl
   ;
   for(auto s : parts)
      actual << s << std::endl;

   cdwAssertEqu(expected.str(),actual.str());
}

cdwTest(point_center)
{
   symbolTable sTable;

   auto pt = argEvaluator(sTable,"pnt[center:tl,dims]{pnt{1,2},rect[tl,br]{pnt{2,2},pnt{4,6}}}").getPoint();

   cdwAssertEqu(2,pt.x);
   cdwAssertEqu(4,pt.y);

   pt = argEvaluator(sTable,"pnt[tl:center,dims]{pnt{2,4},rect[tl,br]{pnt{2,2},pnt{4,6}}}").getPoint();

   cdwAssertEqu(1,pt.x);
   cdwAssertEqu(2,pt.y);
}

#endif // cdwTestBuild
