#include "executor.hpp"
#include "imageCache.hpp"
#include "log.hpp"

bool deferredImageCalculator::shouldSkip(imageCacheAttribute& a, const std::string& path)
{
   if(a.key.empty())
   {
      // first run -- hook the symbol to do the real work and no-op for now
      a.key = path;
      std::unique_ptr<imageCachingSymbol> pVar(new imageCachingSymbol());
      pVar->pAttr = &a;
      m_sTable.overwrite(a.snipName,*pVar.release());
      m_log.s().s() << "deferring load of image - " << path << std::endl;
      return true;
   }
   a.key = path;

   if(a.cache.find(path)!=a.cache.end())
   {
      m_log.s().s() << "skipping reload of already cached image - " << path << std::endl;
      return true;
   }

   return false;
}

void deferredImageCalculator::cacheResult(imageCacheAttribute& a, const std::string& path)
{
   m_log.s().s() << "caching result for " << a.snipName << std::endl;
   auto *pSym = m_pHookedTable->pSymbol;
   a.cache[path] = pSym->getSnippet();
   pSym->getSnippet()->addref();
   delete pSym;
   m_pHookedTable->pSymbol = NULL;
}

iSnippet *deferredImageCalculator::getSnippet(imageCacheAttribute& a)
{
   // try the cache
   auto it = a.cache.find(a.key);
   if(it != a.cache.end())
      return it->second;

   // populate the cache
   m_log.s().s() << "> lazily calculating image - " << a.key << std::endl;
   {
      autoIndent _a(m_log);

      // make a second attributeStore so different graphics attributes are created, e.g.
      // this is important because we're taking an excursion from an unknown place.. i.e.
      // attribute state is already in-use
      attributeStore altAttrStore;
      altAttrStore.overwrite(*a.pEvalTree,&a);

      imageCachingSymbolTable hookedTable(m_log,m_sTable,a.snipName);
      m_pHookedTable = &hookedTable;

      executor exec(m_log,m_errLog,m_gFac,hookedTable);

      {
         attributeStoreBinding _b(a.pEvalTree->root(),altAttrStore);
         a.pEvalTree->acceptVisitor(exec);
      }

      m_pHookedTable = NULL;
      altAttrStore.overwrite<imageCacheAttribute>(*a.pEvalTree,NULL);
   }

   m_log.s().s() << "< done with lazily calcultion - " << a.key << std::endl;

   return a.cache[a.key];
}

imageCacheAttribute::~imageCacheAttribute()
{
   auto it = cache.begin();
   for(;it!=cache.end();++it)
      it->second->release();
}

void imageCacheFinder::visit(loadImageNode& n)
{
   ineligible();
   m_pEvalTree = &n;
   visitChildren(n);
   if(!m_pEvalTree)
      return;

   m_log.s().s() << "flagging an load image to be deferred: " << m_snipName << " => " << m_pEvalTree->path << std::endl;

   auto& attr = n.fetch<imageCacheAttribute>();
   attr.snipName = m_snipName;
   attr.pEvalTree = &n;
   attr.pCalculator = &m_calc;
}

void imageCacheFinder::visit(snipNode& n)
{
   if(!m_pEvalTree)
      return;

   if(!m_snipName.empty())
      ineligible();
   else if(::strchr(n.varName.c_str(),'$'))
   {
      m_log.s().s() << "skipping image cache optization because snip variable is complex: " << n.varName << std::endl;
      ineligible();
   }
   else
      m_snipName = n.varName;
}

void imageCacheFinder::ineligible()
{
   m_pEvalTree = NULL;
   m_snipName = "";
}

iSnippet *imageCachingSymbol::getSnippet()
{
   return pAttr->pCalculator->getSnippet(*pAttr);
}

void imageCachingSymbol::setSnippet(iSnippet& x)
{
   throw std::runtime_error("attempted to change a imageCachingSymbol");
}

void imageCachingSymbolTable::overwrite(const std::string& name, iSymbol& s)
{
   if(name == m_name)
   {
      delete pSymbol;
      pSymbol = &s.as<snipSymbol>();
   }
   else
      m_inner.overwrite(name,s);
}

iSymbol& imageCachingSymbolTable::demand(const std::string& name)
{
   if(name == m_name && pSymbol)
      return *pSymbol;
   else
      return m_inner.demand(name);
}

iSymbol *imageCachingSymbolTable::fetch(const std::string& name)
{
   if(name == m_name && pSymbol)
      return pSymbol;
   else
      return m_inner.fetch(name);
}
