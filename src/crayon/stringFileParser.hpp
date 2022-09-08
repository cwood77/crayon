#pragma once
#include <list>
#include <map>
#include <sstream>
#include <string>

class stringFileParser {
public:
   static void parse(
      const std::string& path,
      std::list<std::string>& schema,
      std::list<std::list<std::string> >& set);

private:
   explicit stringFileParser(std::list<std::list<std::string> >& set)
   : m_set(set), m_recording(false), m_curSectionNum(0) {}

   void categorizeSchema(std::list<std::string>& schema);
   void parseLine(const std::string& line);
   void closeOldTagIf();
   void startNewTag(size_t noob);
   void flushCurrentTags();

   std::list<std::list<std::string> >& m_set;
   std::map<std::string,size_t> m_sectionsInUse;
   std::map<size_t,std::string> m_currentSections;
   bool m_recording;
   size_t m_curSectionNum;
   std::stringstream m_currentSection;
};
