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
   std::unique_ptr<scriptNode> pRoot;

   if(m_l.getCurrentToken() == lexor::kHyphenatedWord && m_l.getCurrentLexeme() == "load-image")
   {
      m_l.advance();
      auto *pNoob = new loadImageNode;
      pNoob->path = m_l.getCurrentLexeme();
      m_l.advance();
      m_l.advance();
      pRoot.reset(pNoob);
      parseImageBlock(*pRoot.get());
   }
   else
      throw std::runtime_error("ise");

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
      pNoob->path = m_l.getCurrentLexeme();
      m_l.advance();
      n.addChild(*pNoob);
      parseImageBlock(n);
   }
}

#ifdef cdwTestBuild

cdwTest(loadsaveimage_parser_acceptance)
{
   std::stringstream program,expected;
   program
      << "load-image \"foo\":" << std::endl
      << "   save-image \"bar\"" << std::endl
      << "   save-image \"bar\"" << std::endl
   ;
   expected
      << "loadImageNode(foo)" << std::endl
      << "saveImageNode(bar)" << std::endl
      << "saveImageNode(bar)" << std::endl
      << "closeImageNode" << std::endl
   ;

   auto copy = program.str();
   lexor l(copy.c_str());
   parser p(l);
   std::unique_ptr<scriptNode> pTree(p.parseFile());

   bufferLog logSink;
   log Log(logSink);
   dumpVisitor dumper(Log);
   pTree->acceptVisitor(dumper);

   cdwAssertEqu(expected.str(),logSink.buffer.str());
}

#endif // cdwTestBuild
