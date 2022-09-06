#include "../crayon/log.hpp"
#include "dumpVisitor.hpp"

void dumpVisitor::visit(scriptNode& n)
{
   m_l.s().s() << "scriptNode" << std::endl;
   visitChildren(n);
}

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

void dumpVisitor::visit(snipNode& n)
{
   m_l.s().s() << "snipNode(" << n.varName << ")" << std::endl;
   visitChildren(n);
}

void dumpVisitor::visit(overlayNode& n)
{
   m_l.s().s() << "overlayNode(" << n.varName << "," << n.transparent << ")" << std::endl;
   visitChildren(n);
}
