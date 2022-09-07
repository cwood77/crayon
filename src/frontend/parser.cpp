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
      if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "define")
      {
         m_l.advance();
         auto *pNoob = new defineNode;

         m_l.demand(lexor::kQuotedText);
         pNoob->varName = m_l.getCurrentLexeme();
         m_l.advance();

         m_l.demand(lexor::kQuotedText,"=");
         m_l.advance();

         m_l.demand(lexor::kQuotedText);
         pNoob->value = m_l.getCurrentLexeme();
         m_l.advance();
         m_root.addChild(*pNoob);
      }
      else if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "load-image")
      {
         m_l.advance();
         auto *pNoob = new loadImageNode;

         m_l.demand(lexor::kQuotedText);
         pNoob->path = m_l.getCurrentLexeme();
         adjustPathIf(pNoob->path);
         m_l.advance();

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
   if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "save-image")
   {
      m_l.advance();
      auto *pNoob = new saveImageNode;

      m_l.demand(lexor::kQuotedText);
      pNoob->path = m_l.getCurrentLexeme();
      adjustPathIf(pNoob->path);
      m_l.advance();

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "snip")
   {
      m_l.advance();
      auto *pNoob = new snipNode;

      m_l.demandAndEat(lexor::kArrow);

      m_l.demand(lexor::kQuotedText);
      pNoob->varName = m_l.getCurrentLexeme();
      m_l.advance();

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "overlay")
   {
      m_l.advance();
      auto *pNoob = new overlayNode;

      m_l.demand(lexor::kQuotedText);
      pNoob->varName = m_l.getCurrentLexeme();
      m_l.advance();

      m_l.demand(lexor::kQuotedText);
      pNoob->transparent = m_l.getCurrentLexeme();
      m_l.advance();

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "remove-frame")
   {
      m_l.advance();
      auto *pNoob = new removeFrameNode;

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
   else if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "select-object")
   {
      m_l.advance();
      auto *pNoob = new selectObjectNode;

      if(m_l.getCurrentToken() == lexor::kQuotedText)
      {
         pNoob->n = m_l.getCurrentLexeme();
         m_l.advance();
      }

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
   else if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "crop")
   {
      m_l.advance();
      auto *pNoob = new cropNode;

      n.addChild(*pNoob);
      parseImageBlock(n);
   }
}

void parser::adjustPathIf(std::string& p)
{
   if(p.empty())
      return;
   if(p.length() > 2 && p.c_str()[1] == ':')
      return; // absolute path

   std::string fullPath = m_scriptPath + "\\..\\" + p;
   p = fullPath;
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
