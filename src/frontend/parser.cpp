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

scriptNode *parser::parseFile()
{
   std::unique_ptr<scriptNode> pRoot(new scriptNode());

   while(m_l.getCurrentToken() != lexor::kEOI)
   {
      if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "load-image")
      {
         m_l.advance();
         auto *pNoob = new loadImageNode;

         m_l.demand(lexor::kQuotedText);
         pNoob->path = m_l.getCurrentLexeme();
         adjustPathIf(pNoob->path);
         m_l.advance();

         m_l.demandAndEat(lexor::kColon);
         pRoot->addChild(*pNoob);
         parseImageBlock(*pNoob);
      }
      else
         throw std::runtime_error("parser error");
   }

   return pRoot.release();
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
   m_l.advance();

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

      m_l.demand(lexor::kColor);
      pNoob->transparent = m_l.getCurrentColorRef();
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
      pNoob->n = 0;

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
   parser p(l,"<mythological script file path>");
   std::unique_ptr<scriptNode> pTree(p.parseFile());

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
   parser p(l,"<mythological script file path>");
   std::unique_ptr<scriptNode> pTree(p.parseFile());

   bufferLog logSink;
   log Log(logSink);
   dumpVisitor dumper(Log);
   pTree->acceptVisitor(dumper);

   cdwAssertEqu(expected.str(),logSink.buffer.str());
}

#endif // cdwTestBuild
