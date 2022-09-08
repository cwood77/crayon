#include "ast.hpp"
#include "lexor.hpp"
#include "parser.hpp"
#include <memory>
#include <stdexcept>

#ifdef cdwTestBuild
#include "../crayon/log.hpp"
#include "../crayon/test.hpp"
#include "dumpVisitor.hpp"
#endif // cdwTestBuild

void parser::parseFile()
{
   fileNode *pFile = new fileNode();
   pFile->scriptPath = m_scriptPath;
   m_root.addChild(*pFile);

   while(m_l.getCurrentToken() != lexor::kEOI)
   {
      if(m_l.isHText("define"))
      {
         m_l.advance();
         auto *pNoob = new defineNode;

         parseArgReq(pNoob->varName);

         m_l.demand(lexor::kQuotedText,"=");
         m_l.advance();

         parseArgReq(pNoob->value);
         pFile->addChild(*pNoob);
      }
      else if(m_l.isHText("load-image"))
      {
         m_l.advance();
         auto *pNoob = new loadImageNode;

         parsePathReq(pNoob->path);

         m_l.demandAndEat(lexor::kColon);
         pFile->addChild(*pNoob);
         m_indent++;
         parseImageBlock(*pNoob);
      }
      else
         throw std::runtime_error("parser error");
   }
}

void parser::parseImageBlock(scriptNode& n)
{
   const size_t myIndent = m_indent;

   if(closeOrContinueBlock(n))
      return;

   if(m_l.isHText("save-image"))
   {
      m_l.advance();
      auto *pNoob = new saveImageNode;

      parsePathReq(pNoob->path);

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.isHText("snip"))
   {
      m_l.advance();
      auto *pNoob = new snipNode;

      m_l.demandAndEat(lexor::kArrow);

      parseArgReq(pNoob->varName);

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.isHText("overlay"))
   {
      m_l.advance();
      auto *pNoob = new overlayNode;

      parseArgReq(pNoob->varName);

      parseArgOpt(pNoob->pnt);

      parseArgOpt(pNoob->transparent);

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.isHText("remove-frame"))
   {
      m_l.advance();
      auto *pNoob = new removeFrameNode;

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.isHText("select-object"))
   {
      m_l.advance();
      auto *pNoob = new selectObjectNode;

      parseArgOpt(pNoob->n);

      parseArgOpt(pNoob->hilight);

      m_l.demandAndEat(lexor::kColon);
      n.addChild(*pNoob);
      m_indent++;
      parseImageBlock(*pNoob);
   }
   else if(m_l.isHText("crop"))
   {
      if(!dynamic_cast<selectObjectNode*>(&n))
         throw std::runtime_error("crop without select-object will have no effect");

      m_l.advance();
      auto *pNoob = new cropNode;

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.isHText("find-whiskers"))
   {
      m_l.advance();
      auto *pNoob = new findWhiskersNode;

      parseArgReq(pNoob->x);

      parseArgReq(pNoob->y);

      m_l.demandAndEat(lexor::kArrow);

      parseArgReq(pNoob->varName);

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.isHText("trim-whiskers"))
   {
      m_l.advance();
      auto *pNoob = new trimWhiskersNode;

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else
      throw std::runtime_error("ise 153");

   // if I just read a line and am still at my same indentation, just
   // loop
   if(m_indent == myIndent)
      parseImageBlock(n);
}

bool parser::closeOrContinueBlock(scriptNode& n)
{
   for(;m_indentsEaten<m_indent;m_indentsEaten++)
   {
      if(m_l.getCurrentToken() != lexor::kIndent)
      {
         // close the block(s)
         auto *pClose = dynamic_cast<iBlockNode&>(n).createCloseNode();
         n.addChild(*pClose);
         m_indent--;
         return true; // remember the indents I've already eaten (m_indentsEaten)
                      // this is only meaningful if m_indent was > 1
                      // if indent = 1, then indentsEaten wil be at most 0 here, so ok!
      }
      m_l.advance(lexor::kAllowComments);
      if(m_l.getCurrentToken() == lexor::kComment)
      {
         m_l.advance();
         // consider this a new line by reseting m_indentsEaten
         // this will be incremented shortly, so go to -1 so I end up at 0
         m_indentsEaten = -1;
      }
   }
   m_indentsEaten = 0;
   return false;
}

void parser::parseArgReq(std::string& arg)
{
   m_l.demand(lexor::kQuotedText);
   arg = m_l.getCurrentLexeme();
   m_l.advance();
}

void parser::parsePathReq(std::string& arg)
{
   if(m_l.getCurrentToken() == lexor::kRelPath)
   {
      arg = m_l.getCurrentLexeme();
      adjustPath(arg);
      m_l.advance();
   }
   else
      parseArgReq(arg);
}

void parser::parseArgOpt(std::string& arg)
{
   if(m_l.getCurrentToken() == lexor::kQuotedText)
   {
      arg = m_l.getCurrentLexeme();
      m_l.advance();
   }
}

void parser::adjustPath(std::string& p)
{
   std::string fullPath = m_scriptPath + "\\..\\" + p;
   p = fullPath;
}

#ifdef cdwTestBuild

cdwTest(loadsaveimage_parser_acceptance)
{
   std::stringstream program,expected;
   program
      << "load-image \"Q:\\foo\":" << std::endl
      << "   save-image \"bar\"" << std::endl
      << "   save-image r\"bar\"" << std::endl
   ;
   expected
      << "scriptNode" << std::endl
      << "   fileNode(<mythological script file path>)" << std::endl
      << "      loadImageNode(Q:\\foo)" << std::endl
      << "         saveImageNode(bar)" << std::endl
      << "         saveImageNode(<mythological script file path>\\..\\bar)" << std::endl
      << "         closeImageNode" << std::endl
   ;

   auto copy = program.str();
   lexor l(copy.c_str());
   std::unique_ptr<scriptNode> pTree(new scriptNode());
   parser p(l,"<mythological script file path>",*pTree.get());
   p.parseFile();

   bufferLog logSink;
   log Log(logSink);
   dumpVisitor dumper(Log);
   pTree->acceptVisitor(dumper);

   cdwAssertEqu(expected.str(),logSink.buffer.str());
}

cdwTest(parser_indent)
{
   std::stringstream program,expected;
   program
      << "load-image \"Q:\\foo\":" << std::endl
      << "   save-image \"Q:\\bar\"" << std::endl
      << "load-image \"Q:\\foo\":" << std::endl
      << "load-image \"Q:\\foo\":" << std::endl
      << "   save-image \"Q:\\bar\"" << std::endl
      << "load-image \"Q:\\foo\":" << std::endl
      << "   select-object \"0\":" << std::endl
      << "      select-object \"0\":" << std::endl
      << "         select-object \"0\":" << std::endl
      << "   save-image \"Q:\\bar\"" << std::endl
   ;
   expected
      << "scriptNode" << std::endl
      << "   fileNode(<mythological script file path>)" << std::endl
      << "      loadImageNode(Q:\\foo)" << std::endl
      << "         saveImageNode(Q:\\bar)" << std::endl
      << "         closeImageNode" << std::endl
      << "      loadImageNode(Q:\\foo)" << std::endl
      << "         closeImageNode" << std::endl
      << "      loadImageNode(Q:\\foo)" << std::endl
      << "         saveImageNode(Q:\\bar)" << std::endl
      << "         closeImageNode" << std::endl
      << "      loadImageNode(Q:\\foo)" << std::endl
      << "         selectObjectNode(0,)" << std::endl
      << "            selectObjectNode(0,)" << std::endl
      << "               selectObjectNode(0,)" << std::endl
      << "                  deselectObjectNode" << std::endl
      << "               deselectObjectNode" << std::endl
      << "            deselectObjectNode" << std::endl
      << "         saveImageNode(Q:\\bar)" << std::endl
      << "         closeImageNode" << std::endl
   ;

   auto copy = program.str();
   lexor l(copy.c_str());
   std::unique_ptr<scriptNode> pTree(new scriptNode());
   parser p(l,"<mythological script file path>",*pTree.get());
   p.parseFile();

   bufferLog logSink;
   log Log(logSink);
   dumpVisitor dumper(Log);
   pTree->acceptVisitor(dumper);

   cdwAssertEqu(expected.str(),logSink.buffer.str());
}

#endif // cdwTestBuild
