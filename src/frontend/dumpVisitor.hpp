#pragma once
#include "ast.hpp"
#include <ostream>

class dumpVisitor : public iNodeVisitor {
public:
   explicit dumpVisitor(std::ostream& s) : m_s(s) {}

   virtual void visit(loadImageNode& n);
   virtual void visit(saveImageNode& n);
   virtual void visit(closeImageNode& n);

private:
   std::ostream& m_s;
};
