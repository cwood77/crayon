#pragma once
#include <map>
#include <string>

class lexor {
public:
   enum tokens {
      kArrow,
      kComment,
      kHyphenatedWord,
      kQuotedText,
      kColon,
      kIndent,
      kRelPath,
      kEOI,
      kIllegal
   };

   enum modes {
      kAllowComments,
      kSuppressComments
   };

   static const char *getTokenName(tokens t);

   explicit lexor(const char *pText);

   tokens getCurrentToken() const { return m_token; }
   const std::string& getCurrentLexeme() const { return m_lexeme; }
   bool isHText(const std::string& text) const
   { return getCurrentToken() == kHyphenatedWord && getCurrentLexeme() == text; }

   void setAdvanceMode(modes m) { m_mode = m; }
   void advance(modes m);
   void advance() { advance(m_mode); }

   void error(const std::string& text);
   void demand(tokens t);
   void demand(tokens t, const std::string& lexeme);
   void demandAndEat(tokens t) { demand(t); advance(); }

private:
   std::map<std::string,tokens> m_words;
   const char *m_pThumb;
   tokens m_token;
   std::string m_lexeme;
   modes m_mode;
};
