#pragma once

// load-image "foo.bmp":
//    save-image "bar.bmp"

class lexor;
class scriptNode;

// <file> ::== 'load-image' <string> ':' <image-block>
//
// <image-block> ::== <indent> 'save-image' <string> <image-block>
//                  | <indent> 'snip' '->' <string> <image-block>
//                  | <indent> 'overlay' <string> <color> <image-block>
//                  | e
//
// <color> ::== 'rgb{' <num> ',' <num> ',' <num> '}'
//
class parser {
public:
   parser(lexor& l, const std::string& scriptPath, scriptNode& root)
   : m_l(l), m_scriptPath(scriptPath), m_root(root) {}

   void parseFile();

private:
   void parseImageBlock(scriptNode& n);

   void adjustPathIf(std::string& p);

   lexor& m_l;
   std::string m_scriptPath;
   scriptNode& m_root;
};
