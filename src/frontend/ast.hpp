#pragma once
#include "attr.hpp"
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

class scriptNode;
class fileNode;
class loadImageNode;
class saveImageNode;
class closeImageNode;
class snipNode;
class overlayNode;
class surveyFrameNode;
class fillNode;
class tightenNode;
class loosenNode;
class desurveyFrameNode;
class selectObjectNode;
class deselectObjectNode;
class cropNode;
class defineNode;
class surveyWhiskersNode;
class findWhiskerPointNode;
class trimWhiskersNode;
class desurveyWhiskersNode;
class foreachStringSetNode;
class closeStringSetNode;
class sweepVarNode;
class closeSweepVarNode;
class echoNode;
class drawTextNode;
class selectFontNode;
class deselectFontNode;
class pixelTransformNode;
class getDimsNode;
class newImageNode;

class iNodeVisitor {
public:
   virtual void visit(scriptNode& n) = 0;
   virtual void visit(fileNode& n) = 0;
   virtual void visit(loadImageNode& n) = 0;
   virtual void visit(saveImageNode& n) = 0;
   virtual void visit(closeImageNode& n) = 0;
   virtual void visit(snipNode& n) = 0;
   virtual void visit(overlayNode& n) = 0;
   virtual void visit(surveyFrameNode& n) = 0;
   virtual void visit(fillNode& n) = 0;
   virtual void visit(tightenNode& n) = 0;
   virtual void visit(loosenNode& n) = 0;
   virtual void visit(desurveyFrameNode& n) = 0;
   virtual void visit(selectObjectNode& n) = 0;
   virtual void visit(deselectObjectNode& n) = 0;
   virtual void visit(cropNode& n) = 0;
   virtual void visit(defineNode& n) = 0;
   virtual void visit(surveyWhiskersNode& n) = 0;
   virtual void visit(findWhiskerPointNode& n) = 0;
   virtual void visit(trimWhiskersNode& n) = 0;
   virtual void visit(desurveyWhiskersNode& n) = 0;
   virtual void visit(foreachStringSetNode& n) = 0;
   virtual void visit(closeStringSetNode& n) = 0;
   virtual void visit(sweepVarNode& n) = 0;
   virtual void visit(closeSweepVarNode& n) = 0;
   virtual void visit(echoNode& n) = 0;
   virtual void visit(drawTextNode& n) = 0;
   virtual void visit(selectFontNode& n) = 0;
   virtual void visit(deselectFontNode& n) = 0;
   virtual void visit(pixelTransformNode& n) = 0;
   virtual void visit(getDimsNode& n) = 0;
   virtual void visit(newImageNode& n) = 0;

protected:
   void visitChildren(scriptNode& n);
};

class scriptNode {
public:
   scriptNode() : pAttrs(NULL), m_pParent(NULL) {}
   virtual ~scriptNode();

   scriptNode *getParent() { return m_pParent; }
   void addChild(scriptNode& n);

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   scriptNode& root()
   {
      if(!m_pParent)
         return *this;
      return m_pParent->root();
   }

   template<class T>
   T& demandAncestor()
   {
      if(!m_pParent)
         throw std::runtime_error("can't find ancestor");
      auto *pAns = dynamic_cast<T*>(m_pParent);
      if(pAns)
         return *pAns;
      else
         return m_pParent->demandAncestor<T>();
   }

   std::vector<scriptNode*>& getChildren() { return m_children; }

   attributeStore *pAttrs;

   template<class A>
   A& fetch() { return root().pAttrs->fetch<A>(*this); }

private:
   scriptNode *m_pParent;
   std::vector<scriptNode*> m_children;
};

class iBlockNode {
public:
   virtual scriptNode *createCloseNode() = 0;
};

class fileNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string scriptPath;
};

class loadImageNode : public scriptNode, public iBlockNode {
public:
   virtual scriptNode *createCloseNode();

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string path;
};

class saveImageNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string path;
};

class closeImageNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class snipNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
   std::string xfrm;
};

class overlayNode : public scriptNode {
public:
   overlayNode() : pnt("pnt{0,0}"), transparent("rgb{255,255,255}") {}

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
   std::string pnt;
   std::string transparent;
};

class surveyFrameNode : public scriptNode, public iBlockNode {
public:
   virtual scriptNode *createCloseNode();
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string color;
};

class fillNode : public scriptNode {
public:
   fillNode() : color("rgb{255,255,255}") {}

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string color;
};

class tightenNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string method;
   std::string arg;
   std::string color;
};

class loosenNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string color;
};

class desurveyFrameNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class selectObjectNode : public scriptNode, public iBlockNode {
public:
   selectObjectNode() : n("0") {}

   virtual scriptNode *createCloseNode();
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string n;
   std::string hilight;
};

class deselectObjectNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class cropNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class defineNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
   std::string value;
};

class surveyWhiskersNode : public scriptNode, public iBlockNode {
public:
   virtual scriptNode *createCloseNode();
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class findWhiskerPointNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string x;
   std::string y;
   std::string varName;
};

class trimWhiskersNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class desurveyWhiskersNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class foreachStringSetNode : public scriptNode, public iBlockNode {
public:
   virtual scriptNode *createCloseNode();
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string filePath;
   std::string schema;
   std::string varName;
};

class closeStringSetNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class sweepVarNode : public scriptNode, public iBlockNode {
public:
   virtual scriptNode *createCloseNode();
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string type;
   std::string start;
   std::string stopOp;
   std::string stopVal;
   std::string delta;
   std::string varName;
};

class closeSweepVarNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class echoNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string text;
};

class drawTextNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string pt;
   std::string text;

   std::list<std::string> options;
};

class selectFontNode : public scriptNode, public iBlockNode {
public:
   virtual scriptNode *createCloseNode();
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string fnt;
   std::string color;
   std::list<std::string> options;
};

class deselectFontNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class pixelTransformNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string op;
   std::string arg;
};

class getDimsNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
};

class newImageNode : public scriptNode, public iBlockNode {
public:
   newImageNode() : color("rgb{255,255,255}") {}

   virtual scriptNode *createCloseNode() { return new closeImageNode(); }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string dims;
   std::string color;
};
