#pragma once
#include "../graphics/graphicsApi.hpp"
#include <list>
#include <string>

class symbolTable;

class argEvaluator {
public:
   argEvaluator(symbolTable& st, const std::string& in)
   : m_sTable(st), m_in(in) {}

   std::string getString();
   size_t getNum();
   size_t getColor();
   point getPoint();
   bool getFlag(const std::string& name);
   std::list<std::string> getSet();
   // getFont

private:
   symbolTable& m_sTable;
   const std::string& m_in;
};
