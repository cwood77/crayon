#include "../crayon/log.hpp"
#include "dumpVisitor.hpp"

void dumpVisitor::visit(scriptNode& n)
{
   m_l.s().s() << indent(m_l) << "scriptNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(fileNode& n)
{
   m_l.s().s() << indent(m_l) << "fileNode(" << n.scriptPath << ")" << std::endl;
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
   m_l.s().s() << indent(m_l) << "selectObjectNode(" << n.n << "," << n.hilight << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(cropNode& n)
{
   m_l.s().s() << indent(m_l) << "cropNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(defineNode& n)
{
   m_l.s().s() << indent(m_l) << "defineNode(" << n.varName << "," << n.value << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(findWhiskersNode& n)
{
   m_l.s().s() << indent(m_l) << "findWhiskersNode(" << n.x << "," << n.y << "," << n.varName << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(trimWhiskersNode& n)
{
   m_l.s().s() << indent(m_l) << "trimWhiskers" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}
