#pragma once

// load-image "foo.bmp":
//    save-image "bar.bmp"

class lexor;
class scriptNode;

// <file> ::== 'define' <qtext> = <qtext> <file>
//           | 'load-image' <qtext> ':' <image-block> <file>
//           | |-
//
// <image-block> ::== <indent> <comment> <image-block>
//                  | <indent> 'save-image' <qtext> <image-block>
//                  | <indent> 'snip' '->' <qtext> <image-block>
//                  | <indent> 'overlay' <qtext> [<qtext>] [<qtext>] <image-block>
//                  | <indent> 'remove-frame' <image-block>
//                  | <indent> 'select-object' [<qtext>] [<qtext>] <image-block>
//                  | <indent> 'crop' <image-block>
//                  | <indent> 'find-whiskers' <qtext> <qtext> '->' <qtext> <image-block>
//                  | <indent> 'trim-whiskers' <image-block>
//                  | e
//
class parser {
public:
   parser(lexor& l, const std::string& scriptPath, scriptNode& root)
   : m_l(l), m_scriptPath(scriptPath), m_root(root) {}

   void parseFile();

private:
   void parseImageBlock(scriptNode& n);

   void parseArgReq(std::string& arg);
   void parsePathReq(std::string& arg);
   void parseArgOpt(std::string& arg);

   void adjustPath(std::string& p);

   lexor& m_l;
   std::string m_scriptPath;
   scriptNode& m_root;
};
