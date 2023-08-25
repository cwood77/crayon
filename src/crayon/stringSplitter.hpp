#pragma once
#include <list>
#include <string>

class stringSplitter {
public:
   static void split(
      const std::string& in,
      std::list<std::string>& schema,
      const std::string& delim,
      std::list<std::string>& values);
};
