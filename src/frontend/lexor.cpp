#define WIN32_LEAN_AND_MEAN
#include "../crayon/test.hpp"
#include "lexor.hpp"
#include <cstring>
#include <windows.h>

static const char *gTokenNames[] = {
   "arrow",
   "color",
   "hyphenword",
   "quotedtext",
   "colon",
   "indent",
   "eoi"
};

const char *lexor::getTokenName(tokens t)
{
   static const size_t nStrings = sizeof(gTokenNames) / sizeof(const char *);
   if((size_t)t >= nStrings)
      throw std::runtime_error("ise");
   return gTokenNames[t];
}

lexor::lexor(const char *pText)
: m_pThumb(pText)
, m_token(kEOI)
, m_color(0)
{
   m_words["load-image"] = kHyphenatedWord;
   m_words["save-image"] = kHyphenatedWord;
   m_words["snip"]       = kHyphenatedWord;
   m_words["overlay"]    = kHyphenatedWord;
   m_words["->"]         = kArrow;
   m_words["rgb{"]       = kColor;
   m_words["   "]        = kIndent;

   advance();
}

void lexor::advance()
{
   m_lexeme.clear();

   for(;*m_pThumb=='\r'||*m_pThumb=='\n';++m_pThumb); // eat newlines

   if(*m_pThumb == ':')
   {
      m_pThumb++;
      m_token = kColon;
   }
   else if(*m_pThumb == 0)
      m_token = kEOI;
   else
   {
      for(size_t i=0;i<2;i++)
      {
         // check word bank
         for(auto it=m_words.begin();it!=m_words.end();++it)
         {
            if(::strncmp(m_pThumb,it->first.c_str(),it->first.length())==0)
            {
               m_pThumb += it->first.length();
               m_lexeme = it->first;
               m_token = it->second;
               if(m_token == kColor)
               {
                  // extra post-processing for colors
                  const char *pEnd = m_pThumb;
                  for(;*pEnd&&*pEnd!='}';++pEnd);
                  if(*pEnd!='}') throw std::runtime_error("unterminated color");
                  unsigned long r,g,b;
                  auto rval = ::sscanf(m_pThumb,"%lu,%lu,%lu",&r,&g,&b);
                  if(rval != 3) throw std::runtime_error("can't parse color");
                  m_color = RGB(r,g,b);
                  m_pThumb = pEnd+1;
               }
               return;
            }
         }

         for(;*m_pThumb==' ';++m_pThumb); // eat whitespace
      }

      // assume quoted string
      m_token = kQuotedText;
      bool hasQuote = (*m_pThumb == '"');
      if(hasQuote)
         m_pThumb++;
      char term = hasQuote ? '"' : ' ';
      const char *pEnd = m_pThumb;
      for(;*pEnd!=0&&*pEnd!='\r'&&*pEnd!='\n'&&*pEnd!=term;++pEnd);
      m_lexeme = std::string(m_pThumb,pEnd-m_pThumb);
      if(hasQuote && *pEnd != '"')
            throw std::runtime_error("unterminated string literal");
      m_pThumb = hasQuote ? pEnd+1 : pEnd;
   }
}

void lexor::demand(tokens t)
{
   if(t != getCurrentToken())
   {
      std::stringstream msg;
      msg << "parser error: expected token " << getTokenName(t)
         << ", but got token " << getTokenName(getCurrentToken())
         << " with '" << getCurrentLexeme() << "'";
      throw std::runtime_error(msg.str().c_str());
   }
}

#ifdef cdwTestBuild

namespace _test {

void lexorToString(lexor& l, std::ostream& s)
{
   while(true)
   {
      s
         << lexor::getTokenName(l.getCurrentToken())
         << "(" << l.getCurrentLexeme() << ")"
         << std::endl
      ;
      if(l.getCurrentToken() == lexor::kEOI)
         break;
      l.advance();
   }
}

} // namespace _test

cdwTest(loadsaveimage_lexor_acceptance)
{
   std::stringstream program;
   program
      << "load-image \"foo\":" << std::endl
      << "   save-image \"bar\"" << std::endl
   ;

   std::stringstream expected,actual;
   expected
      << "hyphenword(load-image)" << std::endl
      << "quotedtext(foo)" << std::endl
      << "colon()" << std::endl
      << "indent(   )" << std::endl
      << "hyphenword(save-image)" << std::endl
      << "quotedtext(bar)" << std::endl
      << "eoi()" << std::endl
   ;

   std::string copy = program.str();
   lexor l(copy.c_str());
   _test::lexorToString(l,actual);
   cdwAssertEqu(expected.str(),actual.str());
}

cdwTest(lexor_color_good)
{
   std::stringstream program;
   program
      << "rgb{1,2,3} -> foo" << std::endl
   ;

   std::stringstream expected,actual;
   expected
      << "color(rgb{)" << std::endl
      << "arrow(->)" << std::endl
      << "quotedtext(foo)" << std::endl
      << "eoi()" << std::endl
   ;

   std::string copy = program.str();
   lexor l(copy.c_str());
   _test::lexorToString(l,actual);
   cdwAssertEqu(expected.str(),actual.str());
   cdwAssertEqu((size_t)RGB(1,2,3),l.getCurrentColorRef());
}

#endif // cdwTestBuild
