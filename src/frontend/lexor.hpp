#pragma once
#include <map>
#include <string>

class lexor {
public:
   enum tokens {
      kArrow,
      kColor,
      kHyphenatedWord,
      kQuotedText,
      kColon,
      kIndent,
      kEOI
   };

   static const char *getTokenName(tokens t);

   explicit lexor(const char *pText);

   tokens getCurrentToken() const { return m_token; }
   const std::string& getCurrentLexeme() const { return m_lexeme; }
   size_t getCurrentLexemeAsNum() const;
   size_t getCurrentColorRef() const { return m_color; }

   void advance();

   void demand(tokens t);
   void demandAndEat(tokens t) { demand(t); advance(); }

private:
   std::map<std::string,tokens> m_words;
   const char *m_pThumb;
   tokens m_token;
   std::string m_lexeme;
   size_t m_color;
};
