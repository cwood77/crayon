#pragma once
#include "../graphics/graphicsApi.hpp"
#include <map>
#include <string>
#include <vector>

class argEvaluator;

class iSymbol {
public:
   virtual ~iSymbol() {}

   template<class T> T& as() { return dynamic_cast<T&>(*this); }

   virtual std::string asString() const = 0;
   virtual iSymbol *clone() const = 0;
};

class stringSymbol : public iSymbol {
public:
   explicit stringSymbol(const std::string& v) : value(v) {}

   std::string value;

   virtual std::string asString() const { return value; }
   virtual iSymbol *clone() const { return new stringSymbol(value); }
};

class snipSymbol : public iSymbol {
public:
   virtual iSnippet *getSnippet() { return m_pSnippet.get(); }
   virtual void setSnippet(iSnippet& x) { m_pSnippet.reset(&x); }

   virtual std::string asString() const;
   virtual iSymbol *clone() const { return new snipSymbol(*this); }

protected:
   autoReleasePtr<iSnippet> m_pSnippet;
};

class iSweepableSymbol : public iSymbol {
public:
   static iSweepableSymbol *create(const std::string& type);
   virtual void start(argEvaluator& e) = 0;
   virtual bool isStop(argEvaluator& op, argEvaluator& val) = 0;
   virtual void adjust(argEvaluator& delta) = 0;
};

class intSymbol : public iSweepableSymbol {
public:
   intSymbol() : value(0) {}

   int value;

   virtual std::string asString() const;
   virtual iSymbol *clone() const { return new intSymbol(*this); }
   virtual void start(argEvaluator& e);
   virtual bool isStop(argEvaluator& op, argEvaluator& val);
   virtual void adjust(argEvaluator& delta);
};

class doubleSymbol : public iSweepableSymbol {
public:
   doubleSymbol() : value(0.0) {}

   double value;

   virtual std::string asString() const;
   virtual iSymbol *clone() const { return new doubleSymbol(*this); }
   virtual void start(argEvaluator& e);
   virtual bool isStop(argEvaluator& op, argEvaluator& val);
   virtual void adjust(argEvaluator& delta);
};

class arraySymbol : public iSymbol {
public:
   ~arraySymbol();
   std::vector<std::map<std::string,iSymbol*> > elts;

   virtual std::string asString() const;
   virtual iSymbol *clone() const;
};

class iSymbolTable {
public:
   virtual ~iSymbolTable() {}
   virtual void overwrite(const std::string& name, iSymbol& s) = 0;
   virtual iSymbol& demand(const std::string& name) = 0;
   virtual iSymbol *fetch(const std::string& name) = 0;
};

class symbolTable : public iSymbolTable {
public:
   ~symbolTable();

   virtual void overwrite(const std::string& name, iSymbol& s);
   virtual iSymbol& demand(const std::string& name);
   virtual iSymbol *fetch(const std::string& name);

private:
   std::map<std::string,iSymbol*> m_pSymbols;
};
