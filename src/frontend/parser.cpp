#include "ast.hpp"
#include "lexor.hpp"
#include "parser.hpp"
#include <memory>
#include <stdexcept>

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
