#define WIN32_LEAN_AND_MEAN
#include "../crayon/test.hpp"
#include "lexor.hpp"
#include <cstring>
#include <windows.h>

static const char *gTokenNames[] = {
   "arrow",
   "comment",
   "hyphenword",
   "quotedtext",
   "colon",
   "indent",
   "relpath",
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
, m_mode(kSuppressComments)
{
   m_words["->"]                = kArrow;
   m_words["   "]               = kIndent;
   m_words["accrue"]            = kHyphenatedWord;
   m_words["box"]               = kHyphenatedWord;
   m_words["crop"]              = kHyphenatedWord;
   m_words["define"]            = kHyphenatedWord;
   m_words["draw-text"]         = kHyphenatedWord;
   m_words["echo"]              = kHyphenatedWord;
   m_words["error"]             = kHyphenatedWord;
   m_words["fill"]              = kHyphenatedWord;
   m_words["find-point"]        = kHyphenatedWord;
   m_words["foreach"]           = kHyphenatedWord;
   m_words["foreach-elt"]       = kHyphenatedWord;
   m_words["foreach-file"]      = kHyphenatedWord;
   m_words["foreach-stringset"] = kHyphenatedWord;
   m_words["get-dims"]          = kHyphenatedWord;
   m_words["if"]                = kHyphenatedWord;
   m_words["load-image"]        = kHyphenatedWord;
   m_words["loosen"]            = kHyphenatedWord;
   m_words["new-image"]         = kHyphenatedWord;
   m_words["nudge"]             = kHyphenatedWord;
   m_words["overlay"]           = kHyphenatedWord;
   m_words["read-tag"]          = kHyphenatedWord;
   m_words["save-image"]        = kHyphenatedWord;
   m_words["select"]            = kHyphenatedWord;
   m_words["select-object"]     = kHyphenatedWord;
   m_words["snip"]              = kHyphenatedWord;
   m_words["split-string"]      = kHyphenatedWord;
   m_words["survey-frame"]      = kHyphenatedWord;
   m_words["survey-objects"]    = kHyphenatedWord;
   m_words["survey-whiskers"]   = kHyphenatedWord;
   m_words["sweep"]             = kHyphenatedWord;
   m_words["tighten"]           = kHyphenatedWord;
   m_words["trim"]              = kHyphenatedWord;
   m_words["with-font"]         = kHyphenatedWord;
   m_words["write-tag"]         = kHyphenatedWord;
   m_words["xfrm-pixels"]       = kHyphenatedWord;
   m_words["anlyz-pixels"]      = kHyphenatedWord;

   advance();
}

void lexor::advance(modes m)
{
   m_lexeme.clear();

   for(;*m_pThumb=='\r'||*m_pThumb=='\n';++m_pThumb); // eat newlines

   if(*m_pThumb == 0)
      m_token = kEOI;
   else
   {
      for(size_t i=0;i<2;i++)
      {
         if(*m_pThumb == ':')
         {
            m_pThumb++;
            m_lexeme = ":";
            m_token = kColon;
            return;
         }

         // check word bank
         for(auto it=m_words.rbegin();it!=m_words.rend();++it)
         {
            if(::strncmp(m_pThumb,it->first.c_str(),it->first.length())==0)
            {
               if(it->second == kIllegal)
                  error("illegal lexeme: " + it->first);

               m_pThumb += it->first.length();
               m_lexeme = it->first;
               m_token = it->second;
               return;
            }
         }

         for(;*m_pThumb==' ';++m_pThumb); // eat whitespace
      }

      // comments
      if(*m_pThumb == '#')
      {
         m_pThumb++;
         // eat until newline
         for(;*m_pThumb!=0&&*m_pThumb!='\r'&&*m_pThumb!='\n';++m_pThumb);
         for(;*m_pThumb=='\r'||*m_pThumb=='\n';++m_pThumb); // eat newlines
         m_token = kComment;

         if(m == kSuppressComments)
            advance(m);

         return;
      }

      // assume quoted string
      bool hasRQuote = (::strncmp(m_pThumb,"r\"",2)==0);
      bool hasQuote = hasRQuote || (*m_pThumb == '"');
      if(hasRQuote)
         m_pThumb+=2;
      else if(hasQuote)
         m_pThumb++;
      char term = hasQuote ? '"' : ' ';
      char colon = hasQuote ? 0 : ':';
      const char *pEnd = m_pThumb;
      for(;*pEnd!=0&&*pEnd!='\r'&&*pEnd!='\n'&&*pEnd!=term&&*pEnd!=colon;++pEnd);
      m_lexeme = std::string(m_pThumb,pEnd-m_pThumb);
      if(hasQuote && *pEnd != '"')
            throw std::runtime_error("unterminated string literal");
      m_pThumb = hasQuote ? pEnd+1 : pEnd;
      m_token = hasRQuote ? kRelPath : kQuotedText;
   }
}

void lexor::error(const std::string& text)
{
   std::stringstream msg;
   msg
      << "parser error: " << text << std::endl
      << "context at time of error:" << std::endl
      << "  token  = " << getTokenName(getCurrentToken()) << std::endl
      << "  lexeme = " << getCurrentLexeme() << std::endl;

   size_t thumbBytes = ::strlen(m_pThumb) > 10 ? 10 : ::strlen(m_pThumb);
   msg
      << "  next " << thumbBytes << " byte(s) of thumb = '"
      << std::string(m_pThumb,thumbBytes) << "'" << std::endl;

   throw std::runtime_error(msg.str().c_str());
}

void lexor::demand(tokens t)
{
   if(t != getCurrentToken())
   {
      std::stringstream msg;
      msg << "expected token " << getTokenName(t);
      error(msg.str());
   }
}

void lexor::demand(tokens t, const std::string& lexeme)
{
   demand(t);
   if(lexeme != getCurrentLexeme())
   {
      std::stringstream msg;
      msg << "parser error: expected lexeme " << lexeme
         << ", but got lexeme " << getCurrentLexeme();
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
      << "   save-image r\"bar\"" << std::endl
   ;

   std::stringstream expected,actual;
   expected
      << "hyphenword(load-image)" << std::endl
      << "quotedtext(foo)" << std::endl
      << "colon(:)" << std::endl
      << "indent(   )" << std::endl
      << "hyphenword(save-image)" << std::endl
      << "relpath(bar)" << std::endl
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
      << "quotedtext(rgb{1,2,3})" << std::endl
      << "arrow(->)" << std::endl
      << "quotedtext(foo)" << std::endl
      << "eoi()" << std::endl
   ;

   std::string copy = program.str();
   lexor l(copy.c_str());
   _test::lexorToString(l,actual);
   cdwAssertEqu(expected.str(),actual.str());
}

cdwTest(lexor_spacesnewlinescomments)
{
   std::stringstream program;
   program
      << "" << std::endl
      << "   # aposdaposdok lkj l " << std::endl
      << "foo # laks" << std::endl
      << "" << std::endl
      << "" << std::endl
      << "bar" << std::endl
   ;

   std::stringstream expectedAllow,expectedSuppress;
   expectedAllow
      << "indent(   )" << std::endl
      << "comment()" << std::endl
      << "quotedtext(foo)" << std::endl
      << "comment()" << std::endl
      << "quotedtext(bar)" << std::endl
      << "eoi()" << std::endl
   ;
   expectedSuppress
      << "indent(   )" << std::endl
      << "quotedtext(foo)" << std::endl
      << "quotedtext(bar)" << std::endl
      << "eoi()" << std::endl
   ;

   std::string copy = program.str();
   {
      std::stringstream actual;
      lexor l(copy.c_str());
      _test::lexorToString(l,actual);
      cdwAssertEqu(expectedSuppress.str(),actual.str());
   }
   {
      std::stringstream actual;
      lexor l(copy.c_str());
      l.setAdvanceMode(lexor::kAllowComments);
      _test::lexorToString(l,actual);
      cdwAssertEqu(expectedAllow.str(),actual.str());
   }
}

#endif // cdwTestBuild
