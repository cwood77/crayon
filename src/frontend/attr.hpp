#pragma once
#include <map>
#include <string>
#include <typeinfo>

class scriptNode;

class attribute {
public:
   virtual ~attribute() {}
};

class attributeStore {
public:
   ~attributeStore();

   template<class T> T& fetch(scriptNode& node)
   {
      attribute*& pA = m_map[&node][typeid(T).name()];
      if(!pA)
         pA = new T();
      return *dynamic_cast<T*>(pA);
   }

private:
   std::map<scriptNode*,std::map<std::string,attribute*> > m_map;
};

class attributeStoreBinding {
public:
   attributeStoreBinding(scriptNode& n, attributeStore& a);
   ~attributeStoreBinding();

private:
   scriptNode& m_n;
};
