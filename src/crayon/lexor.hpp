#pragma once
#include <string>

class lexor {
public:
   enum tokens {
      kHyphenatedWord,
      kQuotedText,
      kColon,
      kIndent,
      kEOI
   };

   static const char *getTokenName(tokens t);

   lexor(const char *pText, size_t lineNum);

   tokens getCurrentToken() const;
   const std::string& getCurrentLexeme() const;

   void advance();
};
