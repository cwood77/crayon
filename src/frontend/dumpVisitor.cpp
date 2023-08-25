#include "../crayon/log.hpp"
#include "dumpVisitor.hpp"

void dumpFieldVisitor::visit(fileNode& n)
{
   m_l.s().r() << "(" << n.scriptPath << ")";
}

void dumpFieldVisitor::visit(defineNode& n)
{
   m_l.s().r() << "(" << n.varName << "," << n.value << ")";
}

void dumpFieldVisitor::visit(foreachStringSetNode& n)
{
   m_l.s().r() << "(" << n.filePath << "," << n.schema << "," << n.varName << ")";
}

void dumpFieldVisitor::visit(echoNode& n)
{
   m_l.s().r() << "(" << n.text << ")";
}

void dumpFieldVisitor::visit(loadImageNode& n)
{
   m_l.s().r() << "(" << n.path << ")";
}

void dumpFieldVisitor::visit(saveImageNode& n)
{
   m_l.s().r() << "(" << n.path << ")";
}

void dumpFieldVisitor::visit(snipNode& n)
{
   m_l.s().r() << "(" << n.varName << ")";
}

void dumpFieldVisitor::visit(overlayNode& n)
{
   m_l.s().r() << "(" << n.varName << "," << n.transparent << ")";
}

void dumpFieldVisitor::visit(selectObjectNodeOLD& n)
{
   m_l.s().r() << "(" << n.n << "," << n.hilight << ")";
}

void dumpFieldVisitor::visit(selectFontNode& n)
{
   m_l.s().r() << "(" << n.fnt << "," << n.options.size() << ")";
}

void dumpFieldVisitor::visit(drawTextNode& n)
{
   m_l.s().r() << "(" << n.pt << "," << n.text << "," << n.options.size() << ")";
}

void dumpFieldVisitor::visit(findWhiskerPointNode& n)
{
   m_l.s().r() << "(" << n.x << "," << n.y << "," << n.varName << ")";
}

void dumpVisitor::visit(scriptNode& n)
{
   m_l.s().s() << "scriptNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

#define cdwAstNode(__type__,__p__) \
void dumpVisitor::visit(__type__& n) \
{ \
   m_l.s().r() << indent(m_l) << n.getName(); \
   dumpFieldVisitor v(m_l); \
   n.acceptVisitor(v); \
   m_l.s().r() << std::endl; \
   autoIndent _i(m_l); \
   visitChildren(n); \
}
#define cdwAstBlockNode(__type__,__p__,__c__) cdwAstNode(__type__,__p__)
#include "ast.info"
#undef cdwAstBlockNode
#undef cdwAstNode
