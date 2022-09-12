#pragma once

// load-image "foo.bmp":
//    save-image "bar.bmp"

class lexor;
class scriptNode;

// <file> ::== 'define' <qtext> = <qtext> <file>
//           | 'load-image' <qtext> ':' <image-block> <file>
//           | <anywhere>
//           | |-
//
// <image-block> ::==
//                  | <indent> 'save-image' <qtext> <image-block>
//                  | <indent> 'snip' '->' <qtext> <image-block>
//                  | <indent> 'overlay' <qtext> [<qtext>] [<qtext>] <image-block>
//                  | <indent> 'survey-frame' ':' <frame-block>
//                  | <indent> 'select-object' [<qtext>] [<qtext>] <image-block>
//                  | <indent> 'crop' <image-block>
//                  | <indent> 'survey-whiskers' ':' <whisker-block>
//                  | <indent> 'draw-text' <qtext> <qtext> <qtext>** <image-block>
//                  | <indent> <anywhere>
//                  | e
//
// <frame-block> ::===
//                  | <indent> 'fill' <frame-block>
//                  | <indent> 'tighten' <frame-block>
//                  | <indent> 'loosen' <frame-block>
//
// <whisker-block> ::==
//                  | <indent> 'find-point' <qtext> <qtext> '->' <qtext> <whisker-block>
//                  | <indent> 'trim' <whisker-block>
//
// <anywhere> ::== 'foreach-stringset' <qtext> <qtext> '->' <qtext>
//               | 'echo' <qtext>
//               | e
//
// <foreach-block> ::==
//                    | <indent> 'load-image' .......
//                    | <indent> <anywhere>
//
class parser {
public:
   parser(lexor& l, const std::string& scriptPath, scriptNode& root)
   : m_l(l), m_scriptPath(scriptPath), m_root(root), m_indent(0), m_indentsEaten(0) {}

   void parseFile();

private:
   void parseImageBlock(scriptNode& n);
   bool closeOrContinueBlock(scriptNode& n);
   void parseFrameBlock(scriptNode& n);
   void parseWhiskerBlock(scriptNode& n);
   void parseForeachBlock(scriptNode& n);
   bool parseAnywhere(scriptNode& n, bool inImageBlock);

   void parseArgReq(std::string& arg);
   void parsePathReq(std::string& arg);
   void parseArgOpt(std::string& arg);

   void adjustPath(std::string& p);

   lexor& m_l;
   std::string m_scriptPath;
   scriptNode& m_root;
   size_t m_indent;
   size_t m_indentsEaten;
};
