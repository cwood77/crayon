#pragma once
#include <map>
#include <string>

class filePathPattern {
public:
   explicit filePathPattern(const std::string& pattern);

   bool isMatch(const std::string& value, std::string& wildcardPart) const;

   std::string prefix;
   bool hasWildcard;
   std::string suffix;
};

class fileFinder {
public:
   static void findAll(
      const std::string& path,
      std::map<std::string,std::string>& results);
};
