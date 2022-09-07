#pragma once
#include "../graphics/graphicsApi.hpp"
#include <map>
#include <string>

class iSymbol {
public:
   virtual ~iSymbol() {}

   template<class T> T& as() { return dynamic_cast<T&>(*this); }

   virtual std::string asString() const = 0;
};

class snipSymbol : public iSymbol {
public:
   autoReleasePtr<iSnippet> pSnippet;

   virtual std::string asString() const;
};

class symbolTable {
public:
   ~symbolTable();

   void overwrite(const std::string& name, iSymbol& s);
   iSymbol& demand(const std::string& name);

private:
   std::map<std::string,iSymbol*> m_pSymbols;
};
