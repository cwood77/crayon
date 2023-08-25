#pragma once
#include "attr.hpp"
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

class scriptNode;
#define cdwAstNode(__type__,__p__) class __type__;
#define cdwAstBlockNode(__type__,__p__,__c__) cdwAstNode(__type__,__p__)
#include "ast.info"
#undef cdwAstBlockNode
#undef cdwAstNode

class iNodeVisitor {
public:
   virtual void visit(scriptNode& n) = 0;
#define cdwAstNode(__type__,__p__) virtual void visit(__type__& n) = 0;
#define cdwAstBlockNode(__type__,__p__,__c__) cdwAstNode(__type__,__p__)
#include "ast.info"
#undef cdwAstBlockNode
#undef cdwAstNode

protected:
   void visitChildren(scriptNode& n);
};

class scriptNode {
public:
   scriptNode() : pAttrs(NULL), m_pParent(NULL) {}
   virtual ~scriptNode();

   scriptNode *getParent() { return m_pParent; }
   void addChild(scriptNode& n);

   virtual const char *getName() const { return "scriptNode"; }
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
   virtual scriptNode *createCloseNode() { return NULL; }
};

class fileNode : public scriptNode {
public:
   virtual const char *getName() const { return "fileNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string scriptPath;
};

// vars -----------------------------------------------------------

class defineNode : public scriptNode {
public:
   virtual const char *getName() const { return "defineNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
   std::string value;
};

class sweepVarNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "sweepVarNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string type;
   std::string start;
   std::string stopOp;
   std::string stopVal;
   std::string delta;
   std::string varName;
};

class foreachStringSetNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "foreachStringSetNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string filePath;
   std::string schema;
   std::string allowMissing;
   std::string varName;
};

class foreachFileNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "foreachFileNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string pattern;
   std::string allowNone;
   std::string varName;
};

class ifNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "ifNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string lhs;
   std::string op;
   std::string rhs;
};

class echoNode : public scriptNode {
public:
   virtual const char *getName() const { return "echoNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string text;
};

class errorNode : public scriptNode {
public:
   virtual const char *getName() const { return "errorNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string text;
};

class accrueNode : public scriptNode {
public:
   virtual const char *getName() const { return "accrueNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string schema;
   std::list<std::string> values;
   std::string varName;
};

class foreachEltNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "foreachEltNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string arrayVarName;
   std::string eltVarName;
};

// images -----------------------------------------------------------

class loadImageNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "loadImageNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual scriptNode *createCloseNode();

   std::string path;
};

class newImageNode : public scriptNode, public iBlockNode {
public:
   newImageNode() : color("rgb{255,255,255}") {}

   virtual const char *getName() const { return "newImageNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual scriptNode *createCloseNode();

   std::string dims;
   std::string color;
};

class closeImageNode : public scriptNode {
public:
   virtual const char *getName() const { return "closeImageNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class saveImageNode : public scriptNode {
public:
   virtual const char *getName() const { return "saveImageNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string path;
};

// cut/paste -----------------------------------------------------------

class snipNode : public scriptNode {
public:
   virtual const char *getName() const { return "snipNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
   std::string xfrm;
};

class overlayNode : public scriptNode {
public:
   overlayNode() : pnt("pnt{0,0}"), transparent("rgb{255,255,255}") {}

   virtual const char *getName() const { return "overlayNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
   std::string pnt;
   std::string transparent;
};

// selection -----------------------------------------------------------

class surveyObjectsNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "surveyObjectsNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual scriptNode *createCloseNode();
};

class desurveyObjectsNode : public scriptNode {
public:
   virtual const char *getName() const { return "desurveyObjectsNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class foreachObjectNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "foreachObjectNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
};

class selectObjectNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "selectObjectNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual scriptNode *createCloseNode();

   std::string method;
   std::string arg;
};

class selectObjectNodeOLD : public scriptNode, public iBlockNode {
public:
   selectObjectNodeOLD() : n("0") {}

   virtual const char *getName() const { return "selectObjectNodeOLD"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual scriptNode *createCloseNode();

   std::string n;
   std::string hilight;
   std::string withTags;
};

class deselectObjectNode : public scriptNode {
public:
   virtual const char *getName() const { return "deselectObjectNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class getDimsNode : public scriptNode {
public:
   virtual const char *getName() const { return "getDimsNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string obj;
   std::string varName;
};

class boxNode : public scriptNode {
public:
   virtual const char *getName() const { return "boxNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string outlineCol;
   std::string rect;
   std::string fillCol;
};

class cropNode : public scriptNode {
public:
   virtual const char *getName() const { return "cropNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

// text -----------------------------------------------------------

class selectFontNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "selectFontNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual scriptNode *createCloseNode();

   std::string fnt;
   std::string color;
   std::list<std::string> options;
};

class deselectFontNode : public scriptNode {
public:
   virtual const char *getName() const { return "deselectFontNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class drawTextNode : public scriptNode {
public:
   virtual const char *getName() const { return "drawTextNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string pt;
   std::string text;

   std::list<std::string> options;
};

// outlines -----------------------------------------------------------

class surveyFrameNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "surveyFrameNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual scriptNode *createCloseNode();

   std::string color;
};

class desurveyFrameNode : public scriptNode {
public:
   virtual const char *getName() const { return "desurveyFrameNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class fillNode : public scriptNode {
public:
   fillNode() : color("rgb{255,255,255}") {}

   virtual const char *getName() const { return "fillNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string color;
};

class tightenNode : public scriptNode {
public:
   virtual const char *getName() const { return "tightenNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string method;
   std::string arg;
   std::string color;
};

class loosenNode : public scriptNode {
public:
   virtual const char *getName() const { return "loosenNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string color;
};

// demarcation -----------------------------------------------------------

class surveyWhiskersNode : public scriptNode, public iBlockNode {
public:
   virtual const char *getName() const { return "surveyWhiskersNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual scriptNode *createCloseNode();
};

class desurveyWhiskersNode : public scriptNode {
public:
   virtual const char *getName() const { return "desurveyWhiskersNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class findWhiskerPointNode : public scriptNode {
public:
   virtual const char *getName() const { return "findWhiskerPointNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string x;
   std::string y;
   std::string varName;
};

class trimWhiskersNode : public scriptNode {
public:
   virtual const char *getName() const { return "trimWhiskersNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class nudgeNode : public scriptNode {
public:
   virtual const char *getName() const { return "nudgeNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string mode;
   std::string in;
   std::string amt;
   std::string varName;
};

// transforms -----------------------------------------------------------

class pixelTransformNode : public scriptNode {
public:
   virtual const char *getName() const { return "pixelTransformNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string op;
   std::string arg;
   std::string arg2;
};

class pixelAnalysisNode : public scriptNode {
public:
   virtual const char *getName() const { return "pixelAnalysisNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string op;
   std::string arg;
   std::string varName;
};

// tags -----------------------------------------------------------

class writeTagNode : public scriptNode {
public:
   virtual const char *getName() const { return "writeTagNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string text;
};

class readTagNode : public scriptNode {
public:
   virtual const char *getName() const { return "readTagNode"; }
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
};

// impl block nodes
#define cdwAstNode(__type__,__p__)
#define cdwAstBlockNode(__type__,__p__,__c__) \
inline scriptNode *__type__::createCloseNode() { return new __c__(); }
#include "ast.info"
#undef cdwAstBlockNode
#undef cdwAstNode

class nodeVisitor : public iNodeVisitor {
public:
   virtual void visit(scriptNode& n) {}
#define cdwAstNode(__type__,__p__) virtual void visit(__type__& n) {}
#define cdwAstBlockNode(__type__,__p__,__c__) cdwAstNode(__type__,__p__)
#include "ast.info"
#undef cdwAstBlockNode
#undef cdwAstNode
};
