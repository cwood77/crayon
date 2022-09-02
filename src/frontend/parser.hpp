#pragma once

// load-image "foo.bmp":
//    save-image "bar.bmp"

class lexor;
class scriptNode;

// <file> ::== 'load-image' <string> ':' <image-block>
//
// <image-block> ::== <indent> 'save-image' <string> <image-block>
//
class parser {
public:
   explicit parser(lexor& l) : m_l(l) {}

   scriptNode *parseFile();

private:
   void parseImageBlock(scriptNode& n);

   lexor& m_l;
};
