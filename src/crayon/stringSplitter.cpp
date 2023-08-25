#include "stringSplitter.hpp"
#include "test.hpp"
#include <cstring>
#include <stdexcept>

void stringSplitter::split(const std::string& in, std::list<std::string>& schema, const std::string& delim, std::list<std::string>& values)
{
   const char *pThumb = in.c_str();
   const char *pStart = pThumb;
   for(;;++pThumb)
   {
      const bool hitDelim = (::strncmp(pThumb,delim.c_str(),delim.length())==0);
      if(*pThumb==0 || hitDelim)
      {
         std::string word(pStart,pThumb-pStart);
         values.push_back(word);
         if(hitDelim)
         {
            pThumb += delim.length();
            pStart = pThumb;
         }
         else
            break;
      }
   }

   if(values.size() != schema.size())
      throw std::runtime_error("got an unexpected number of values when parsing");
}

#ifdef cdwTestBuild

cdwTest(stringSplitter_acceptance)
{
   std::list<std::string> schema;
   schema.push_back("foo");
   schema.push_back("bar");
   schema.push_back("baz");

   std::list<std::string> values, expected;
   stringSplitter::split("a-b-c",schema,"-",values);

   expected.push_back("a");
   expected.push_back("b");
   expected.push_back("c");

   if(expected != values)
   {
      ::printf("something is wrong; here's what I got\n");
      for(auto x : values)
         ::printf("<%s>\n",x.c_str());
      cdwAssertEqu("true","false");
   }
}

#endif // cdwTestBuild
