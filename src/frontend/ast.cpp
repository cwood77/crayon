#include "ast.hpp"

void iNodeVisitor::visitChildren(scriptNode& n)
{
   auto& children = n.getChildren();
   for(auto *pChild : children)
      pChild->acceptVisitor(*this);
}

scriptNode::~scriptNode()
{
   for(auto pChild : m_children)
      delete pChild;
   m_children.clear();
}

void scriptNode::addChild(scriptNode& n)
{
   m_children.push_back(&n);
   n.m_pParent = this;
}

scriptNode *loadImageNode::createCloseNode()
{
   return new closeImageNode();
}

scriptNode *selectObjectNode::createCloseNode()
{
   return new deselectObjectNode();
}

scriptNode *foreachStringSetNode::createCloseNode()
{
   return new closeStringSetNode();
}
