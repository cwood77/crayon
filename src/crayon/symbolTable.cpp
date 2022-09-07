#include "symbolTable.hpp"
#include <stdexcept>

std::string snipSymbol::asString() const
{
   throw std::runtime_error("can't convert a snippet to a string");
}

symbolTable::~symbolTable()
{
   for(auto it=m_pSymbols.begin();it!=m_pSymbols.end();++it)
      delete it->second;
}

void symbolTable::overwrite(const std::string& name, iSymbol& s)
{
   iSymbol*& pOld = m_pSymbols[name];
   if(pOld)
      delete pOld;
   pOld = &s;
}

iSymbol& symbolTable::demand(const std::string& name)
{
   auto it = m_pSymbols.find(name.c_str()+1);
   if(it == m_pSymbols.end())
      throw std::runtime_error("referenced variable doesn't exist");
   return *it->second;
}
