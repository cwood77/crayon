#pragma once
#include "../frontend/ast.hpp"
#include "../graphics/graphicsApi.hpp"

class log;

class graphicsAttribute : public attribute {
public:
   autoReleasePtr<iGraphicsApi> pApi;
   autoReleasePtr<iBitmap> pImage;
};

class executor : public iNodeVisitor {
public:
   executor(log& l, graphicsApiFactory& g) : m_log(l), m_gFac(g) {}

   virtual void visit(scriptNode& n) { visitChildren(n); }
   virtual void visit(loadImageNode& n);
   virtual void visit(saveImageNode& n);
   virtual void visit(closeImageNode& n);
   virtual void visit(snipNode& n);
   virtual void visit(overlayNode& n);

private:
   log& m_log;
   graphicsApiFactory& m_gFac;
};
