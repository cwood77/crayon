#define WIN32_LEAN_AND_MEAN
#include "path.hpp"
#include <stdexcept>
#include <string>
#include <windows.h>

namespace {

void _lower(char& c)
{
   if('A' <= c && c <= 'Z')
      c += ('z' - 'Z');
}

};

std::string getPathExtLowered(const std::string& path)
{
   const char *pEnd = path.c_str() + path.length() - 1;
   for(;*pEnd&&*pEnd!='.';--pEnd);
   if(*pEnd!='.')
      throw std::runtime_error("path must have an extension");
   std::string ext = pEnd+1;

   char *pStr = const_cast<char*>(ext.c_str());
   for(size_t i=0;i<ext.length();++i)
      _lower(pStr[i]);

   return ext;
}

std::wstring widen(const std::string& s)
{
   /*
   size_t rval; // includes NULL termin
   wchar_t destPtr; // always terminated
   size_t destSizeInWords;
   char *srcPtr;
   size_t numCharsExcludingNull;
   ::mbstowcs_s(&rval,&destPtr,destSizeInWords,srcPtr,numCharsExcludingNull);
   */
   wchar_t buffer[MAX_PATH];
   ::mbstowcs_s(NULL,buffer,MAX_PATH,s.c_str(),MAX_PATH-1);
   return std::wstring(buffer);
}

std::string createTempFilePath(const std::string& sSeed, size_t nSeed)
{
   char path[MAX_PATH];
   ::GetTempPath(MAX_PATH,path);

   char file[MAX_PATH];
   ::GetTempFileName(path,sSeed.c_str(),nSeed,file);

   return file;
}

autoDeleteFile::~autoDeleteFile()
{
   ::DeleteFileA(path.c_str());
}
