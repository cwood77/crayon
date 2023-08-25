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
      bool allowMissing,
      std::list<std::list<std::string> >& set);

private:
   stringFileParser(std::list<std::list<std::string> >& set, bool allowMissing)
   : m_set(set), m_allowMissing(allowMissing), m_recording(false), m_curSectionNum(0) {}

   void categorizeSchema(std::list<std::string>& schema);
   void parseLine(const std::string& line);
   void closeOldTagIf();
   void startNewTag(size_t noob);
   void flushCurrentTags();
   void fillInMissing();

   std::list<std::list<std::string> >& m_set;
   const bool m_allowMissing;
   std::map<std::string,size_t> m_sectionsInUse;
   std::map<size_t,std::string> m_currentSections;
   bool m_recording;
   size_t m_curSectionNum;
   std::stringstream m_currentSection;
};
