#pragma once
#include <stdexcept>
#include <string>
#include <vector>

class scriptNode;
class loadImageNode;
class saveImageNode;
class closeImageNode;

class iNodeVisitor {
public:
   virtual void visit(loadImageNode& n) = 0;
   virtual void visit(saveImageNode& n) = 0;
   virtual void visit(closeImageNode& n) = 0;

protected:
   void visitChildren(scriptNode& n);
};

class scriptNode {
public:
   scriptNode() : m_pParent(NULL) {}
   virtual ~scriptNode();

   scriptNode *getParent() { return m_pParent; }
   void addChild(scriptNode& n);

   virtual void acceptVisitor(iNodeVisitor& v) = 0;

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
