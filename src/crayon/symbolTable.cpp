#include "../frontend/eval.hpp"
#include "symbolTable.hpp"
#include <sstream>
#include <stdexcept>

std::string snipSymbol::asString() const
{
   throw std::runtime_error("can't convert a snippet to a string");
}

iSweepableSymbol *iSweepableSymbol::create(const std::string& type)
{
   if(type == "real")
      return new doubleSymbol;
   else
      throw std::runtime_error("var type not supported");
}

std::string doubleSymbol::asString() const
{
   std::stringstream stream;
   stream << value;
   return stream.str();
}

void doubleSymbol::start(argEvaluator& e)
{
   value = e.getReal();
}

bool doubleSymbol::isStop(argEvaluator& op, argEvaluator& val)
{
   if(op.getString() == "<")
      return !(value < val.getReal());
   else if(op.getString() == ">")
      return !(value > val.getReal());
   else
      throw std::runtime_error("comparison op not supported");
}

void doubleSymbol::adjust(argEvaluator& delta)
{
   value += delta.getReal();
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
