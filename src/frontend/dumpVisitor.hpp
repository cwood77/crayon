#pragma once
#include "ast.hpp"
#include <ostream>

class log;

class dumpFieldVisitor : public nodeVisitor {
public:
   explicit dumpFieldVisitor(log& l) : m_l(l) {}

   virtual void visit(fileNode& n);
   virtual void visit(defineNode& n);
   virtual void visit(foreachStringSetNode& n);
   virtual void visit(echoNode& n);
   virtual void visit(loadImageNode& n);
   virtual void visit(saveImageNode& n);
   virtual void visit(snipNode& n);
   virtual void visit(overlayNode& n);
   virtual void visit(selectObjectNodeOLD& n);
   virtual void visit(selectFontNode& n);
   virtual void visit(drawTextNode& n);
   virtual void visit(findWhiskerPointNode& n);

private:
   log& m_l;
};

class dumpVisitor : public iNodeVisitor {
public:
   explicit dumpVisitor(log& l) : m_l(l) {}

   virtual void visit(scriptNode& n);
#define cdwAstNode(__type__,__p__) virtual void visit(__type__& n);
#define cdwAstBlockNode(__type__,__p__,__c__) cdwAstNode(__type__,__p__)
#include "ast.info"
#undef cdwAstBlockNode
#undef cdwAstNode

private:
   log& m_l;
};
