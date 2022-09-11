#pragma once
#include "../graphics/graphicsApi.hpp"
#include <map>
#include <string>

class argEvaluator;

class iSymbol {
public:
   virtual ~iSymbol() {}

   template<class T> T& as() { return dynamic_cast<T&>(*this); }

   virtual std::string asString() const = 0;
};

class stringSymbol : public iSymbol {
public:
   explicit stringSymbol(const std::string& v) : value(v) {}

   std::string value;

   virtual std::string asString() const { return value; }
};

class snipSymbol : public iSymbol {
public:
   autoReleasePtr<iSnippet> pSnippet;

   virtual std::string asString() const;
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
   virtual void start(argEvaluator& e);
   virtual bool isStop(argEvaluator& op, argEvaluator& val);
   virtual void adjust(argEvaluator& delta);
};

class doubleSymbol : public iSweepableSymbol {
public:
   doubleSymbol() : value(0.0) {}

   double value;

   virtual std::string asString() const;
   virtual void start(argEvaluator& e);
   virtual bool isStop(argEvaluator& op, argEvaluator& val);
   virtual void adjust(argEvaluator& delta);
};

class symbolTable {
public:
   ~symbolTable();

   void overwrite(const std::string& name, iSymbol& s);
   iSymbol& demand(const std::string& name);

private:
   std::map<std::string,iSymbol*> m_pSymbols;
};
