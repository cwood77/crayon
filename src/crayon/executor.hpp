#pragma once
#include "../frontend/ast.hpp"
#include "../graphics/graphicsApi.hpp"

class log;
class symbolTable;

class graphicsAttribute : public attribute {
public:
   autoReleasePtr<iGraphicsApi> pApi;
   autoReleasePtr<iBitmap> pImage;
   autoReleasePtr<iCanvas> pCanvas;
};

class executor : public iNodeVisitor {
public:
   executor(log& l, graphicsApiFactory& g, symbolTable& sTable)
   : m_log(l), m_gFac(g), m_sTable(sTable) {}

   virtual void visit(scriptNode& n) { visitChildren(n); }
   virtual void visit(fileNode& n) { visitChildren(n); }
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

private:
   log& m_log;
   graphicsApiFactory& m_gFac;
   symbolTable& m_sTable;
};
