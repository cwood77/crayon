#include "dumpVisitor.hpp"

void dumpVisitor::visit(loadImageNode& n)
{
   m_s << "loadImageNode(" << n.path << ")" << std::endl;
   visitChildren(n);
}

void dumpVisitor::visit(saveImageNode& n)
{
   m_s << "saveImageNode(" << n.path << ")" << std::endl;
   visitChildren(n);
}

void dumpVisitor::visit(closeImageNode& n)
{
   m_s << "closeImageNode" << std::endl;
   visitChildren(n);
}
