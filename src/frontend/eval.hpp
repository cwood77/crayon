#pragma once
#include "../graphics/graphicsApi.hpp"
#include <list>
#include <map>
#include <string>

class symbolTable;

class argEvaluator {
public:
   static size_t computeBitFlags(
      symbolTable& st,
      const std::list<std::string>& options,
      std::map<std::string,size_t>& table);

   argEvaluator(symbolTable& st, const std::string& in)
   : m_sTable(st), m_in(in) {}

   std::string getString();
   size_t getNum();
   size_t getColor();
   point getPoint();
   bool getFlag(const std::string& name);
   std::list<std::string> getSet();
   size_t lookup(std::map<std::string,size_t>& table);

private:
   symbolTable& m_sTable;
   const std::string& m_in;
};
