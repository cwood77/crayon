#pragma once
#include "attr.hpp"
#include <stdexcept>
#include <string>
#include <vector>

class scriptNode;
class loadImageNode;
class saveImageNode;
class closeImageNode;
class snipNode;
class overlayNode;
class removeFrameNode;
class selectObjectNode;

class iNodeVisitor {
public:
   virtual void visit(scriptNode& n) = 0;
   virtual void visit(loadImageNode& n) = 0;
   virtual void visit(saveImageNode& n) = 0;
   virtual void visit(closeImageNode& n) = 0;
   virtual void visit(snipNode& n) = 0;
   virtual void visit(overlayNode& n) = 0;
   virtual void visit(removeFrameNode& n) = 0;
   virtual void visit(selectObjectNode& n) = 0;

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
         return pAns;
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

class loadImageNode : public scriptNode {
public:
   closeImageNode *createCloseNode();

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
};

class overlayNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   std::string varName;
   size_t transparent;
};

class removeFrameNode : public scriptNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class selectObjectNode : public scriptNode {
public:
   selectObjectNode() : n(0) {}

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   size_t n;
};
