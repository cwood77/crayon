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
   virtual void visit(surveyFrameNode& n) { throw std::runtime_error("unimpled"); }
   virtual void visit(fillNode& n) { throw std::runtime_error("unimpled"); }
   virtual void visit(tightenNode& n) { throw std::runtime_error("unimpled"); }
   virtual void visit(loosenNode& n) { throw std::runtime_error("unimpled"); }
   virtual void visit(desurveyFrameNode& n) { throw std::runtime_error("unimpled"); }
   virtual void visit(selectObjectNode& n);
   virtual void visit(deselectObjectNode& n);
   virtual void visit(cropNode& n);
   virtual void visit(defineNode& n);
   virtual void visit(surveyWhiskersNode& n);
   virtual void visit(findWhiskerPointNode& n);
   virtual void visit(trimWhiskersNode& n);
   virtual void visit(desurveyWhiskersNode& n);
   virtual void visit(foreachStringSetNode& n);
   virtual void visit(closeStringSetNode& n);
   virtual void visit(echoNode& n);
   virtual void visit(drawTextNode& n);
   virtual void visit(selectFontNode& n);
   virtual void visit(deselectFontNode& n);

private:
   log& m_l;
};
