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

size_t argEvaluator::computeBitFlags(symbolTable& st, const std::list<std::string>& options, std::map<std::string,size_t>& table)
{
   size_t ans = 0;
   for(auto opt : options)
      ans |= argEvaluator(st,opt).lookup(table);
   return ans;
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

size_t argEvaluator::getNum()
{
   std::string in = getString();

   size_t x;
   ::sscanf(in.c_str(),"%llu",&x);
   return x;
}

size_t argEvaluator::getColor()
{
   std::string in = getString();

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

   if(::strncmp(in.c_str(),"pnt{",4)!=0)
      throw std::runtime_error("invalid point syntax");
   unsigned long x,y;
   auto rval = ::sscanf(in.c_str()+4,"%lu,%lu",&x,&y);
   if(rval != 2) throw std::runtime_error("can't parse point");
   return point(x,y);
}

bool argEvaluator::getFlag(const std::string& name)
{
   std::string in = getString();
   return in == name;
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

size_t argEvaluator::lookup(std::map<std::string,size_t>& table)
{
   std::string in = getString();
   auto it = table.find(in);
   if(it == table.end())
      throw std::runtime_error("arg value not in table");
   return it->second;
}

void argEvaluator::getFont(std::string& face, size_t& pnt)
{
   std::string in = getString();

   if(::strncmp(in.c_str(),"font{\"",6)!=0)
      throw std::runtime_error("invalid font syntax");
   char buffer[MAX_PATH];
   auto rval = ::sscanf(in.c_str()+6,"%[^\"]\",%llu",buffer,&pnt);
   if(rval != 2) throw std::runtime_error("can't parse font");
   face = buffer;
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
   expandInterpolationParts("font{{$typeface},10}",parts);

   std::stringstream expected,actual;
   expected
      << "font{" << std::endl
      << "$typeface" << std::endl
      << ",10}" << std::endl
   ;
   for(auto s : parts)
      actual << s << std::endl;

   cdwAssertEqu(expected.str(),actual.str());
}

#endif // cdwTestBuild
