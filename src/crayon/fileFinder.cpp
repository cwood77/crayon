#define WIN32_LEAN_AND_MEAN
#include "fileFinder.hpp"
#include "path.hpp"
#include "test.hpp"
#include <windows.h>

filePathPattern::filePathPattern(const std::string& pattern)
{
   const char *pThumb = pattern.c_str();
   for(;*pThumb&&*pThumb!='*';++pThumb);
   if(*pThumb == 0)
   {
      prefix = pattern;
      hasWildcard = false;
      return;
   }
   prefix = std::string(pattern.c_str(),pThumb-pattern.c_str());
   hasWildcard = true;
   suffix = pThumb + 1;
}

bool filePathPattern::isMatch(const std::string& value, std::string& wildcardPart) const
{
   if(::strnicmp(prefix.c_str(),value.c_str(),prefix.length())!=0)
      return false;

   size_t nRest = value.length() - prefix.length();
   if(nRest < suffix.length())
      return false;
   if(hasWildcard && nRest == suffix.length())
      return false;
   if(::strnicmp(suffix.c_str(),value.c_str()+value.length()-suffix.length(),suffix.length())!=0)
      return false;

   wildcardPart = std::string(
      value.c_str() + prefix.length(),
      value.length() - prefix.length() - suffix.length());

   return true;
}

void fileFinder::findAll(const std::string& path, std::map<std::string,std::string>& results)
{
   filePathPattern patt(path);
   auto dir = getPathDirIf(path);

   WIN32_FIND_DATA fData;
   HANDLE hFind = ::FindFirstFileA(
      (dir + "\\*").c_str(),
      &fData);
   if(hFind == INVALID_HANDLE_VALUE)
      return;
   do
   {
      auto fullPath = dir + "\\" + fData.cFileName;
      if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
         continue;

      std::string var;
      if(patt.isMatch(fullPath,var))
         results[fullPath] = var;

   } while(::FindNextFile(hFind,&fData));
   ::FindClose(hFind);
}

#ifdef cdwTestBuild

cdwTest(filePathPattern_split_typical)
{
   filePathPattern pattern("dir\\card-*.bmp");
   cdwAssertEqu("dir\\card-",pattern.prefix);
   cdwAssertEqu(true,pattern.hasWildcard);
   cdwAssertEqu(".bmp",pattern.suffix);
}

cdwTest(filePathPattern_split_noprefix)
{
   filePathPattern pattern("*.bmp");
   cdwAssertEqu("",pattern.prefix);
   cdwAssertEqu(true,pattern.hasWildcard);
   cdwAssertEqu(".bmp",pattern.suffix);
}

cdwTest(filePathPattern_split_nosuffix)
{
   filePathPattern pattern("dir\\card-*");
   cdwAssertEqu("dir\\card-",pattern.prefix);
   cdwAssertEqu(true,pattern.hasWildcard);
   cdwAssertEqu("",pattern.suffix);
}

cdwTest(filePathPattern_split_nowildcard)
{
   filePathPattern pattern("dir\\card.bmp");
   cdwAssertEqu("dir\\card.bmp",pattern.prefix);
   cdwAssertEqu(false,pattern.hasWildcard);
   cdwAssertEqu("",pattern.suffix);
}

cdwTest(filePathPattern_match_prefix)
{
   filePathPattern pattern("dir\\card-*.bmp");
   std::string out;
   bool m = pattern.isMatch("card-01.bmp",out); // prefix mismatch
   cdwAssertEqu(false,m);
   cdwAssertEqu(out,"");

   m = pattern.isMatch("DiR\\card-01.bmp",out); // case insensitive
   cdwAssertEqu(true,m);
   cdwAssertEqu(out,"01");
}

cdwTest(filePathPattern_match_suffix)
{
   filePathPattern pattern("dir\\card-*.bmp");
   std::string out;
   bool m = pattern.isMatch("dir\\card-01.png",out); // prefix mismatch
   cdwAssertEqu(false,m);
   cdwAssertEqu(out,"");

   m = pattern.isMatch("dir\\card-01.BMP",out); // case insensitive
   cdwAssertEqu(true,m);
   cdwAssertEqu(out,"01");
}

cdwTest(filePathPattern_match_noWildcard)
{
   filePathPattern pattern("dir\\card.bmp");
   std::string out;
   bool m = pattern.isMatch("dir\\card.png",out); // mismatch
   cdwAssertEqu(false,m);
   cdwAssertEqu(out,"");

   m = pattern.isMatch("dir\\card.BMP",out); // case insensitive
   cdwAssertEqu(true,m);
   cdwAssertEqu(out,"");
}

cdwTest(filePathPattern_match_noSuffix)
{
   filePathPattern pattern("dir\\card.*");
   std::string out;
   bool m = pattern.isMatch("dir\\card.png",out);
   cdwAssertEqu(true,m);
   cdwAssertEqu(out,"png");
}

#endif // cdwTestBuild
