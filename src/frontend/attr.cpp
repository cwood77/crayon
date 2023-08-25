#include "../crayon/test.hpp"
#include "ast.hpp"
#include "attr.hpp"

attributeStore::~attributeStore()
{
   for(auto it=m_map.begin();it!=m_map.end();++it)
      for(auto jit=it->second.begin();jit!=it->second.end();++jit)
         delete jit->second;
}

attributeStoreBinding::attributeStoreBinding(scriptNode& n, attributeStore& a)
: m_n(n)
, m_pOldStore(n.pAttrs)
{
   m_n.pAttrs = &a;
}

attributeStoreBinding::~attributeStoreBinding()
{
   m_n.pAttrs = m_pOldStore;
}

#ifdef cdwTestBuild

namespace _test {

class intAttribute : public attribute {
public:
   intAttribute() : i(0) {}
   int i;
};

} // namespace _test

cdwTest(attribute_acceptance)
{
   scriptNode root;
   auto *pLoad = new loadImageNode();
   root.addChild(*pLoad);

   attributeStore as;
   attributeStoreBinding b(root,as);

   {
      auto& a = pLoad->fetch<_test::intAttribute>();
      cdwAssertEqu(0,a.i);
      a.i = 7;
   }
   {
      auto& a = pLoad->fetch<_test::intAttribute>();
      cdwAssertEqu(7,a.i);
   }
   {
      auto& a = pLoad->fetch<_test::intAttribute>();
      cdwAssertEqu(7,a.i);
   }
   {
      auto& a = root.fetch<_test::intAttribute>();
      cdwAssertEqu(0,a.i);
   }
}

#endif // cdwTestBuild
