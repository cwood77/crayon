#pragma once
#include <string>

std::string getPathExtLowered(const std::string& path);
std::wstring widen(const std::string& s);
std::string createTempFilePath(const std::string& sSeed, size_t nSeed);

class autoDeleteFile {
public:
   explicit autoDeleteFile(const std::string& path) : path(path) {}

   ~autoDeleteFile();

   std::string path;
};
