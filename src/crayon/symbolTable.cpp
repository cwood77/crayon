#include "../frontend/eval.hpp"
#include "symbolTable.hpp"
#include <sstream>
#include <stdexcept>

std::string snipSymbol::asString() const
{
   long w,h;
   m_pSnippet->getDims(w,h);
   return argEvaluator::fmtRect(rect(0,0,w,h));
}

iSweepableSymbol *iSweepableSymbol::create(const std::string& type)
{
   if(type == "real")
      return new doubleSymbol;
   else if(type == "int")
      return new intSymbol;
   else
      throw std::runtime_error("var type not supported");
}

std::string intSymbol::asString() const
{
   std::stringstream stream;
   stream << value;
   return stream.str();
}

void intSymbol::start(argEvaluator& e)
{
   value = e.getInt();
}

bool intSymbol::isStop(argEvaluator& op, argEvaluator& val)
{
   if(op.getString() == "<")
      return !(value < val.getInt());
   else if(op.getString() == ">")
      return !(value > val.getInt());
   else
      throw std::runtime_error("comparison op not supported");
}

void intSymbol::adjust(argEvaluator& delta)
{
   value += delta.getInt();
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

arraySymbol::~arraySymbol()
{
   for(auto it=elts.begin();it!=elts.end();++it)
      for(auto jit=it->begin();jit!=it->end();++jit)
         delete jit->second;
}

std::string arraySymbol::asString() const
{
   throw std::runtime_error("can't convert an array to a string");
}

iSymbol *arraySymbol::clone() const
{
   throw std::runtime_error("unimpled 82");
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
   auto *pS = fetch(name.c_str()+1);
   if(!pS)
      throw std::runtime_error("referenced variable doesn't exist: " + name);
   return *pS;
}

iSymbol *symbolTable::fetch(const std::string& name)
{
   auto it = m_pSymbols.find(name.c_str());
   if(it == m_pSymbols.end())
      return NULL;
   return it->second;
}
