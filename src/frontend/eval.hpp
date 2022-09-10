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

   // variable expansion
   std::string getString();

   // pods
   size_t getNum();
   bool getFlag(const std::string& name);
   size_t lookup(std::map<std::string,size_t>& table);

   // for stringset
   std::list<std::string> getSet();

   // graphics types
   size_t getColor();
   point getPoint();
   void getFont(std::string& face, size_t& pnt);

private:
   symbolTable& m_sTable;
   const std::string& m_in;
};
