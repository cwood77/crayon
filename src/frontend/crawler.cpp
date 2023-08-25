#define WIN32_LEAN_AND_MEAN
#include "../crayon/log.hpp"
#include "../crayon/test.hpp"
#include "crawler.hpp"
#include <windows.h>

namespace {

std::string assemblePathParts(std::list<std::string>& words, size_t n)
{
   std::stringstream stream;
   for(auto it=words.begin();n;++it,--n)
   {
      if(!stream.str().empty())
         stream << "\\";
      stream << *it;
   }
   return stream.str();
}

void computeParentPaths(const std::string& basePath, const std::string& scriptPath, std::list<std::string>& parents)
{
   // compute an adjustedPath that is the absolute path to the script
   // if the script is provied as relative, then use the pwd (basePath) to absolutize
   std::string adjustedPath = scriptPath;
   if(adjustedPath.length() < 2 || adjustedPath.c_str()[1] != ':')
   {
      if(basePath.c_str()[basePath.length()-1] == '\\')
         adjustedPath = basePath + scriptPath;
      else
         adjustedPath = basePath + "\\" + scriptPath;
   }

   std::list<std::string> words;

   const char *pThumb = adjustedPath.c_str();
   const char *pStart = pThumb;
   for(;true;++pThumb)
   {
      if(*pThumb == '\\' || *pThumb == '/' || *pThumb == 0)
      {
         std::string word(pStart,pThumb-pStart);

         if(word == ".")
            ;
         else if(word == "..")
            words.pop_back();
         else
            words.push_back(word);

         if(*pThumb == 0)
            break;
         pStart = pThumb+1;
         pThumb = pStart;
      }
   }

   size_t n = words.size() - 1; // ignore the script file name word
   for(;n;n--)
      parents.push_front(assemblePathParts(words,n));
}

} // anonymous namespace

void crawler::crawl(const std::string& scriptPath, std::list<std::string>& addtlScripts, log& Log)
{
   Log.s().s() << "crawling up dirtree from " << scriptPath << std::endl;

   char basePath[MAX_PATH];
   ::GetCurrentDirectoryA(MAX_PATH,basePath);
   std::list<std::string> searchPaths;
   computeParentPaths(basePath,scriptPath,searchPaths);

   for(auto searchPath : searchPaths)
   {
      Log.s().s() << "searching " << searchPath << std::endl;

      std::set<std::string> files;
      WIN32_FIND_DATA fData;
      HANDLE hFind = ::FindFirstFile((searchPath + "\\*.crayi").c_str(),&fData);
      if(hFind == INVALID_HANDLE_VALUE)
         continue;
      do
      {
         std::string fullPath = searchPath + "\\" + fData.cFileName;
         Log.s().s() << "  found " << fullPath << std::endl;
         files.insert(fullPath);
      } while(::FindNextFile(hFind,&fData));

      // use a set just in case FindFirstFile isn't returning in alpha order
      for(auto& file : files)
         addtlScripts.push_back(file);
   }
}

#ifdef cdwTestBuild

cdwTest(computeParentPaths_absolute)
{
   std::list<std::string> parents;
   computeParentPaths("Y:\\","X:\\1\\2\\3\\4.cray",parents);

   std::stringstream expected,actual;
   expected
      << "X:" << std::endl
      << "X:\\1" << std::endl
      << "X:\\1\\2" << std::endl
      << "X:\\1\\2\\3" << std::endl
   ;
   for(auto s : parents)
      actual << s << std::endl;

   cdwAssertEqu(expected.str(),actual.str());
}

cdwTest(computeParentPaths_relative)
{
   std::list<std::string> parents;
   computeParentPaths("Y:\\1","3/4.cray",parents);

   std::stringstream expected,actual;
   expected
      << "Y:" << std::endl
      << "Y:\\1" << std::endl
      << "Y:\\1\\3" << std::endl
   ;
   for(auto s : parents)
      actual << s << std::endl;

   cdwAssertEqu(expected.str(),actual.str());
}

cdwTest(computeParentPaths_relative_rootDir)
{
   std::list<std::string> parents;
   computeParentPaths("Y:\\","3/4.cray",parents);

   std::stringstream expected,actual;
   expected
      << "Y:" << std::endl
      << "Y:\\3" << std::endl
   ;
   for(auto s : parents)
      actual << s << std::endl;

   cdwAssertEqu(expected.str(),actual.str());
}

#endif // cdwTestBuild
