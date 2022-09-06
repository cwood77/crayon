#include "../crayon/log.hpp"
#include "dumpVisitor.hpp"

void dumpVisitor::visit(scriptNode& n)
{
   m_l.s().s() << indent(m_l) << "scriptNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(loadImageNode& n)
{
   m_l.s().s() << indent(m_l) << "loadImageNode(" << n.path << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(saveImageNode& n)
{
   m_l.s().s() << indent(m_l) << "saveImageNode(" << n.path << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(closeImageNode& n)
{
   m_l.s().s() << indent(m_l) << "closeImageNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(snipNode& n)
{
   m_l.s().s() << indent(m_l) << "snipNode(" << n.varName << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(overlayNode& n)
{
   m_l.s().s() << indent(m_l) << "overlayNode(" << n.varName << "," << n.transparent << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(removeFrameNode& n)
{
   m_l.s().s() << indent(m_l) << "removeFrameNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(selectObjectNode& n)
{
   m_l.s().s() << indent(m_l) << "selectObjectNode(" << n.n << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}
