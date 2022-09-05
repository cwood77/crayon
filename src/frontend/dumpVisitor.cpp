#include "../crayon/log.hpp"
#include "dumpVisitor.hpp"

void dumpVisitor::visit(loadImageNode& n)
{
   m_l.s().s() << "loadImageNode(" << n.path << ")" << std::endl;
   visitChildren(n);
}

void dumpVisitor::visit(saveImageNode& n)
{
   m_l.s().s() << "saveImageNode(" << n.path << ")" << std::endl;
   visitChildren(n);
}

void dumpVisitor::visit(closeImageNode& n)
{
   m_l.s().s() << "closeImageNode" << std::endl;
   visitChildren(n);
}
