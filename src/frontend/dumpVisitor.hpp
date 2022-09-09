#pragma once
#include "ast.hpp"
#include <ostream>

class log;

class dumpVisitor : public iNodeVisitor {
public:
   explicit dumpVisitor(log& l) : m_l(l) {}

   virtual void visit(scriptNode& n);
   virtual void visit(fileNode& n);
   virtual void visit(loadImageNode& n);
   virtual void visit(saveImageNode& n);
   virtual void visit(closeImageNode& n);
   virtual void visit(snipNode& n);
   virtual void visit(overlayNode& n);
   virtual void visit(removeFrameNode& n);
   virtual void visit(selectObjectNode& n);
   virtual void visit(deselectObjectNode& n);
   virtual void visit(cropNode& n);
   virtual void visit(defineNode& n);
   virtual void visit(findWhiskersNode& n);
   virtual void visit(trimWhiskersNode& n);
   virtual void visit(foreachStringSetNode& n);
   virtual void visit(closeStringSetNode& n);
   virtual void visit(echoNode& n);
   virtual void visit(drawTextNode& n);

private:
   log& m_l;
};
