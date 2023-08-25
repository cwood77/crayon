#pragma once
#include "../graphics/graphicsApi.hpp"
#include <list>
#include <map>
#include <string>

class iSymbolTable;

class argEvaluator {
public:
   static size_t computeBitFlags(
      iSymbolTable& st,
      const std::list<std::string>& options,
      std::map<std::string,size_t>& table);

   static std::string fmtColor(size_t c);
   static std::string fmtPoint(const point& p);
   static std::string fmtRect(const rect& r);

   argEvaluator(iSymbolTable& st, const std::string& in)
   : m_sTable(st), m_in(in) {}

   // variable expansion
   std::string getString();

   // PODs
   long getInt();
   double getReal();
   bool getFlag(const std::string& name);
   size_t lookup(std::map<std::string,size_t>& table);

   // for stringset
   std::list<std::string> getSet();

   // graphics types
   size_t getColor();
   point getPoint();
   rect getRect();
   colorRange getColorRange();
   void getFont(std::string& face, size_t& pnt);

   // measurement
   size_t getPixelCount();

private:
   void parsePointAndRect(const std::string& text, unsigned long& x, unsigned long& y, rect& r);

   iSymbolTable& m_sTable;
   const std::string& m_in;
};
