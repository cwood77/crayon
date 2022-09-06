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
   parser(lexor& l, const std::string& scriptPath) : m_l(l), m_scriptPath(scriptPath) {}

   scriptNode *parseFile();

private:
   void parseImageBlock(scriptNode& n);

   void adjustPathIf(std::string& p);

   lexor& m_l;
   std::string m_scriptPath;
};
