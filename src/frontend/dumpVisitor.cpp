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

void dumpVisitor::visit(selectObjectNode& n)
{
   m_l.s().s() << indent(m_l) << "selectObjectNode(" << n.n << "," << n.hilight << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(deselectObjectNode& n)
{
   m_l.s().s() << indent(m_l) << "deselectObjectNode" << std::endl;
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

void dumpVisitor::visit(surveyWhiskersNode& n)
{
   m_l.s().s() << indent(m_l) << "findWhiskersNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(findWhiskerPointNode& n)
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

void dumpVisitor::visit(desurveyWhiskersNode& n)
{
   m_l.s().s() << indent(m_l) << "desurveyWhiskerNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(foreachStringSetNode& n)
{
   m_l.s().s() << indent(m_l) << "foreachStringSetNode(" << n.filePath << "," << n.schema << "," << n.varName << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(closeStringSetNode& n)
{
   m_l.s().s() << indent(m_l) << "closeStringSetNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(echoNode& n)
{
   m_l.s().s() << indent(m_l) << "echoNode(" << n.text << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(drawTextNode& n)
{
   m_l.s().s() << indent(m_l) << "drawTextNode(" << n.pt << "," << n.text << "," << n.options.size() << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(selectFontNode& n)
{
   m_l.s().s() << indent(m_l) << "selectFontNode(" << n.fnt << "," << n.options.size() << ")" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(deselectFontNode& n)
{
   m_l.s().s() << indent(m_l) << "deselectFontNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(getDimsNode& n)
{
   m_l.s().s() << indent(m_l) << "getDimsNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(endIfNode& n)
{
   m_l.s().s() << indent(m_l) << "endIfNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(ifNode& n)
{
   m_l.s().s() << indent(m_l) << "ifNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}

void dumpVisitor::visit(errorNode& n)
{
   m_l.s().s() << indent(m_l) << "errorNode" << std::endl;
   autoIndent _i(m_l);
   visitChildren(n);
}
