#pragma once
#include "../frontend/ast.hpp"
#include "../graphics/algorithm.hpp"
#include "../graphics/graphicsApi.hpp"
#include <memory>

class log;
class symbolTable;

class graphicsAttribute : public attribute {
public:
   autoReleasePtr<iGraphicsApi> pApi;
   autoReleasePtr<iBitmap> pImage;
   autoReleasePtr<iCanvas> pCanvas;
   autoReleasePtr<iFont>   pFont;
};

class objectAttribute : public attribute {
public:
   std::unique_ptr<objectSurvey> pObjects;
};

class frameAttribute : public attribute {
public:
   std::unique_ptr<framer> pFramer;
};

class whiskerAttribute : public attribute {
public:
   std::unique_ptr<whiskerSurvey> pSurvey;
};

class executor : public iNodeVisitor {
public:
   executor(log& l, log& errLog, graphicsApiFactory& g, symbolTable& sTable)
   : m_log(l), m_errLog(errLog), m_gFac(g), m_sTable(sTable) {}

   virtual void visit(scriptNode& n) { visitChildren(n); }
   virtual void visit(fileNode& n) { visitChildren(n); }

   virtual void visit(defineNode& n);
   virtual void visit(sweepVarNode& n);
   virtual void visit(foreachStringSetNode& n);
   virtual void visit(ifNode& n);
   virtual void visit(echoNode& n);
   virtual void visit(errorNode& n);

   virtual void visit(loadImageNode& n);
   virtual void visit(newImageNode& n);
   virtual void visit(closeImageNode& n);
   virtual void visit(saveImageNode& n);

   virtual void visit(snipNode& n);
   virtual void visit(overlayNode& n);

   virtual void visit(surveyObjectsNode& n);
   virtual void visit(desurveyObjectsNode& n);
   virtual void visit(selectObjectNode& n);
   virtual void visit(selectObjectNodeOLD& n);
   virtual void visit(deselectObjectNode& n);
   virtual void visit(getDimsNode& n);
   virtual void visit(cropNode& n);

   virtual void visit(selectFontNode& n);
   virtual void visit(deselectFontNode& n);
   virtual void visit(drawTextNode& n);

   virtual void visit(surveyFrameNode& n);
   virtual void visit(desurveyFrameNode& n);
   virtual void visit(fillNode& n);
   virtual void visit(tightenNode& n);
   virtual void visit(loosenNode& n);

   virtual void visit(surveyWhiskersNode& n);
   virtual void visit(desurveyWhiskersNode& n);
   virtual void visit(findWhiskerPointNode& n);
   virtual void visit(trimWhiskersNode& n);

   virtual void visit(pixelTransformNode& n);

   virtual void visit(writeTagNode& n);
   virtual void visit(readTagNode& n);

private:
   std::string trimTrailingNewlines(const std::string& s);

   log& m_log;
   log& m_errLog;
   graphicsApiFactory& m_gFac;
   symbolTable& m_sTable;
};
