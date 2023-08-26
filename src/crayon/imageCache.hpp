#pragma once
#include "../frontend/ast.hpp"
#include "symbolTable.hpp"
#include <memory>

// idea is: lets find blocks like:
//
// load-image
//    snip -> foo
// foreach-file ...
//    
// and defer/cache them.  Deferring helps scenarios where foreach-file finds nothing.
// Caching helps scenarios where the same image could be reloaded, like this:
//
// foreach-stringset ...
//    load-image $strs.part
//       snip

class executor;
class graphicsApiFactory;
class imageCacheAttribute;
class imageCachingSymbolTable;
class log;

// big throbbing brain
class deferredImageCalculator {
public:
   deferredImageCalculator(log& l, log& errLog, graphicsApiFactory& g, iSymbolTable& sTable)
   : m_log(l), m_errLog(errLog), m_gFac(g), m_sTable(sTable), m_pHookedTable(NULL) {}

   bool shouldSkip(imageCacheAttribute& a, const std::string& path);
   void cacheResult(imageCacheAttribute& a, const std::string& path);
   iSnippet *getSnippet(imageCacheAttribute& a);

private:
   log& m_log;
   log& m_errLog;
   graphicsApiFactory& m_gFac;
   iSymbolTable& m_sTable;
   imageCachingSymbolTable *m_pHookedTable;
};

// stashed on load-image attributes; maintains cache state
class imageCacheAttribute : public attribute {
public:
   imageCacheAttribute() : pEvalTree(NULL), pCalculator(NULL) {}
   ~imageCacheAttribute();

   bool isCached() const { return pEvalTree != NULL; }

   std::string snipName;
   loadImageNode *pEvalTree;
   deferredImageCalculator *pCalculator;

   std::string key;
   std::map<std::string,iSnippet*> cache;
};

// marks load-image nodes as cached, if appropriate
// intentionally limited in what it allows
class imageCacheFinder : public iNodeVisitor {
public:
   imageCacheFinder(deferredImageCalculator& d, log& l)
   : m_calc(d), m_log(l) { ineligible(); }

   virtual void visit(scriptNode& n) { visitChildren(n); }
   virtual void visit(fileNode& n) { visitChildren(n); }
   virtual void visit(defineNode& n) { ineligible(); }
   virtual void visit(sweepVarNode& n) { ineligible(); visitChildren(n); }
   virtual void visit(foreachStringSetNode& n) { ineligible(); visitChildren(n); }
   virtual void visit(foreachFileNode& n) { ineligible(); visitChildren(n); }
   virtual void visit(ifNode& n) { ineligible(); visitChildren(n); }
   virtual void visit(echoNode& n) { ineligible(); }
   virtual void visit(errorNode& n) { ineligible(); }
   virtual void visit(accrueNode& n) { ineligible(); }
   virtual void visit(splitNode& n) { ineligible(); }
   virtual void visit(foreachEltNode& n) { ineligible(); visitChildren(n); }
   virtual void visit(loadImageNode& n);
   virtual void visit(newImageNode& n) { ineligible(); visitChildren(n); }
   virtual void visit(closeImageNode& n) { allowed(); }
   virtual void visit(saveImageNode& n) { ineligible(); }
   virtual void visit(snipNode& n);
   virtual void visit(overlayNode& n) { allowed(); }
   virtual void visit(surveyObjectsNode& n) { allowed(); visitChildren(n); }
   virtual void visit(desurveyObjectsNode& n) { allowed(); }
   virtual void visit(foreachObjectNode& n) { ineligible(); visitChildren(n); }
   virtual void visit(selectObjectNode& n) { allowed(); visitChildren(n); }
   virtual void visit(selectObjectNodeOLD& n) { allowed(); visitChildren(n); }
   virtual void visit(deselectObjectNode& n) { allowed(); }
   virtual void visit(getDimsNode& n) { ineligible(); }
   virtual void visit(boxNode& n) { allowed(); }
   virtual void visit(cropNode& n) { allowed(); }
   virtual void visit(selectFontNode& n) { allowed(); visitChildren(n); }
   virtual void visit(deselectFontNode& n) { allowed(); }
   virtual void visit(drawTextNode& n) { allowed(); }
   virtual void visit(surveyFrameNode& n) { allowed(); visitChildren(n); }
   virtual void visit(desurveyFrameNode& n) { allowed(); }
   virtual void visit(fillNode& n) { allowed(); }
   virtual void visit(tightenNode& n) { allowed(); }
   virtual void visit(loosenNode& n) { allowed(); }
   virtual void visit(selectInsideFrameNode& n) { allowed(); visitChildren(n); }
   virtual void visit(surveyWhiskersNode& n) { allowed(); visitChildren(n); }
   virtual void visit(desurveyWhiskersNode& n) { allowed(); }
   virtual void visit(findWhiskerPointNode& n) { ineligible(); }
   virtual void visit(trimWhiskersNode& n) { allowed(); }
   virtual void visit(nudgeNode& n) { ineligible(); }
   virtual void visit(pixelTransformNode& n) { allowed(); }
   virtual void visit(pixelAnalysisNode& n) { allowed(); }
   virtual void visit(writeTagNode& n) { ineligible(); }
   virtual void visit(readTagNode& n) { ineligible(); }

private:
   void allowed() {}
   void ineligible();

   deferredImageCalculator& m_calc;
   log& m_log;
   loadImageNode *m_pEvalTree;
   std::string m_snipName;
};

// this symbol looks like a snippet, but when accesses
// triggers the system to actually execute
class imageCachingSymbol : public snipSymbol {
public:
   imageCachingSymbol() : pAttr(NULL) {}

   imageCacheAttribute *pAttr;

   virtual iSnippet *getSnippet();
   virtual void setSnippet(iSnippet& x);
};

// this symbol table is temporarily injected when running
// the deferred code for real; this allows the cache to
// capture the snippet result while also preventing the hooked
// symbol from being overwritten
class imageCachingSymbolTable : public iSymbolTable {
public:
   imageCachingSymbolTable(log& l, iSymbolTable& inner, const std::string& name)
   : pSymbol(NULL), m_log(l), m_inner(inner), m_name(name) {}

   ~imageCachingSymbolTable() { delete pSymbol; }

   snipSymbol *pSymbol;

   virtual void overwrite(const std::string& name, iSymbol& s);
   virtual iSymbol& demand(const std::string& name);
   virtual iSymbol *fetch(const std::string& name);

private:
   log& m_log;
   iSymbolTable& m_inner;
   std::string m_name;
};
