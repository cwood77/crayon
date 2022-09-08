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
         m_root.addChild(*pNoob);
      }
      else if(m_l.isHText("load-image"))
      {
         m_l.advance();
         auto *pNoob = new loadImageNode;

         parseArgReq(pNoob->path);
         adjustPathIf(pNoob->path);

         m_l.demandAndEat(lexor::kColon);
         m_root.addChild(*pNoob);
         parseImageBlock(*pNoob);
      }
      else
         throw std::runtime_error("parser error");
   }
}

void parser::parseImageBlock(scriptNode& n)
{
   if(m_l.getCurrentToken() != lexor::kIndent)
   {
      // close the block
      auto *pClose = dynamic_cast<loadImageNode&>(n).createCloseNode();
      n.addChild(*pClose);
      return;
   }
   m_l.advance(lexor::kAllowComments);

   if(m_l.getCurrentToken() == lexor::kComment)
   {
      m_l.advance();
      parseImageBlock(n);
   }
   else if(m_l.isHText("save-image"))
   {
      m_l.advance();
      auto *pNoob = new saveImageNode;

      parseArgReq(pNoob->path);
      adjustPathIf(pNoob->path);

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

      // TODO
      if(m_l.getCurrentToken() == lexor::kQuotedText)
      {
         if(m_l.getCurrentLexeme() != "hilight")
            throw std::runtime_error("usage: select-object [#] [hilight]");
         pNoob->dbgHilight = true;
         m_l.advance();
      }

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.isHText("crop"))
   {
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
}

// TODO
void parser::adjustPathIf(std::string& p)
{
   if(p.empty())
      return;
   if(p.length() > 2 && p.c_str()[1] == ':')
      return; // absolute path

   std::string fullPath = m_scriptPath + "\\..\\" + p;
   p = fullPath;
}

void parser::parseArgReq(std::string& arg)
{
   m_l.demand(lexor::kQuotedText);
   arg = m_l.getCurrentLexeme();
   m_l.advance();
}

void parser::parseArgOpt(std::string& arg)
{
   if(m_l.getCurrentToken() == lexor::kQuotedText)
   {
      arg = m_l.getCurrentLexeme();
      m_l.advance();
   }
}

#ifdef cdwTestBuild

cdwTest(loadsaveimage_parser_acceptance)
{
   std::stringstream program,expected;
   program
      << "load-image \"Q:\\foo\":" << std::endl
      << "   save-image \"Q:\\bar\"" << std::endl
      << "   save-image \"bar\"" << std::endl
   ;
   expected
      << "scriptNode" << std::endl
      << "   loadImageNode(Q:\\foo)" << std::endl
      << "      saveImageNode(Q:\\bar)" << std::endl
      << "      saveImageNode(<mythological script file path>\\..\\bar)" << std::endl
      << "      closeImageNode" << std::endl
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
   ;
   expected
      << "scriptNode" << std::endl
      << "   loadImageNode(Q:\\foo)" << std::endl
      << "      saveImageNode(Q:\\bar)" << std::endl
      << "      closeImageNode" << std::endl
      << "   loadImageNode(Q:\\foo)" << std::endl
      << "      closeImageNode" << std::endl
      << "   loadImageNode(Q:\\foo)" << std::endl
      << "      saveImageNode(Q:\\bar)" << std::endl
      << "      closeImageNode" << std::endl
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
