#include "stringFileParser.hpp"
#include "test.hpp"
#include <cstring>
#include <fstream>

void stringFileParser::parse(const std::string& path, std::list<std::string>& schema, std::list<std::list<std::string> >& set)
{
   stringFileParser self(set);
   self.categorizeSchema(schema);
   std::ifstream stream(path.c_str());
   if(!stream.good())
      throw std::runtime_error("can't open string file");

   while(stream.good())
   {
      std::string line;
      std::getline(stream,line);
      self.parseLine(line);
   }
   self.closeOldTagIf();
   self.flushCurrentTags();
}

void stringFileParser::categorizeSchema(std::list<std::string>& schema)
{
   for(auto s : schema)
      m_sectionsInUse[s] = m_sectionsInUse.size();
}

void stringFileParser::parseLine(const std::string& line)
{
   if(::strncmp(line.c_str(),"{{",2)==0)
   {
      // this is a tag line
      const char *pThumb = line.c_str()+2;
      for(;*pThumb!=0&&*pThumb!='}';++pThumb);
      if(::strncmp(pThumb,"}}",2)!=0)
         throw std::runtime_error("malformed schema line in strings file");
      std::string schemaTag(line.c_str()+2,pThumb-line.c_str()-2);

      auto it = m_sectionsInUse.find(schemaTag);
      bool inUse = (it != m_sectionsInUse.end());

      closeOldTagIf();

      // switch to new tag
      if(inUse)
         startNewTag(it->second);

      m_recording = inUse;
   }
   else if(m_recording)
   {
      // this is a non-tag line
      m_currentSection << line << std::endl;
   }
}

void stringFileParser::closeOldTagIf()
{
   if(m_recording)
      m_currentSections[m_curSectionNum] = m_currentSection.str();
}

void stringFileParser::startNewTag(size_t noob)
{
   m_curSectionNum = noob;
   m_currentSection.str("");

   // flush the tagset, if this is a repeat
   if(m_currentSections.find(m_curSectionNum)!=m_currentSections.end())
      flushCurrentTags();
}

void stringFileParser::flushCurrentTags()
{
   if(m_currentSections.size()==0)
      return;

   if(m_currentSections.size() != m_sectionsInUse.size())
      throw std::runtime_error("one of the requested tags was not found");

   std::list<std::string> set;
   for(auto it=m_currentSections.begin();it!=m_currentSections.end();++it)
      set.push_back(it->second);
   m_set.push_back(set);
   m_currentSections.clear();
}

#ifdef cdwTestBuild

// this verifies:
//   - trailing (ignored) pre-text
//   - tags may be out of order
//   - extra tags may be present but are ignored if not requested
cdwTest(stringFileParser_acceptance)
{
   std::list<std::string> schema;
   schema.push_back("foo");
   schema.push_back("bar");

   std::list<std::list<std::string> > set,expected;
   stringFileParser::parse("testdata\\unittest-strings.txt",schema,set);

   {
      std::list<std::string> x;
      {
         std::stringstream s;
         s
            << "alas, poor yorick" << std::endl
            << "I knew him" << std::endl
            << "" << std::endl
            << "Horatio" << std::endl
            << "" << std::endl
         ;
         x.push_back(s.str());
      }
      {
         std::stringstream s;
         s
            << "Get thee to a Nunnery!" << std::endl
            << "" << std::endl
         ;
         x.push_back(s.str());
      }
      expected.push_back(x);
   }
   {
      std::list<std::string> x;
      {
         std::stringstream s;
         s
            << "the sledded pollacks" << std::endl
            << "" << std::endl
         ;
         x.push_back(s.str());
      }
      {
         std::stringstream s;
         s
            << "" << std::endl
            << "leigeman to the dane!" << std::endl
            << "" << std::endl
         ;
         x.push_back(s.str());
      }
      expected.push_back(x);
   }

   if(expected != set)
   {
      ::printf("something is wrong; here's what I got\n");
      for(auto x : set)
         for(auto s : x)
            ::printf("<%s>\n",s.c_str());
      cdwAssertEqu("true","false");
   }
}

// this verifies:
//   - requested tokens _must_ be present in every record
cdwTest(stringFileParser_illegal)
{
   std::list<std::string> schema;
   schema.push_back("foo");
   schema.push_back("baz");

   try
   {
      std::list<std::list<std::string> > set;
      stringFileParser::parse("testdata\\unittest-strings.txt",schema,set);
      cdwAssertEqu("true","false");
   }
   catch(std::exception& x)
   {
      cdwAssertEqu("true","true");
   }
}

#endif // cdwTestBuild
