#include "../graphics/algorithm.hpp"
#include "../graphics/snippet.hpp"
#include "executor.hpp"
#include "log.hpp"
#include "symbolTable.hpp"
#include <memory>

void executor::visit(loadImageNode& n)
{
   m_log.s().s() << "loading image '" << n.path << "'" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   // open the API
   if(attr.pApi)
      throw std::runtime_error("graphics API already in use during load");
   attr.pApi.reset(m_gFac.open(0));

   // load the file
   autoReleasePtr<iFileType> pBmpFmt(attr.pApi->createFileType(0));
   attr.pImage.reset(pBmpFmt->loadBitmap(n.path.c_str()));
   attr.pCanvas.reset(attr.pImage.get());

   visitChildren(n);
}

void executor::visit(saveImageNode& n)
{
   m_log.s().s() << "saving image '" << n.path << "'" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   autoReleasePtr<iFileType> pBmpFmt(attr.pApi->createFileType(0));
   pBmpFmt->saveBitmap(attr.pImage,n.path.c_str());

   visitChildren(n);
}

void executor::visit(closeImageNode& n)
{
   m_log.s().s() << "closing image" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   attr.pCanvas.reset();
   attr.pImage.reset();
   attr.pApi.reset();

   visitChildren(n);
}

void executor::visit(snipNode& n)
{
   m_log.s().s() << "snipping image" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   std::unique_ptr<snipSymbol> pVar(new snipSymbol());

   snippetAllocator sAlloc;
   nullTransform nullXfrm;
   pVar->pSnippet.reset(attr.pCanvas->snip(sAlloc,nullXfrm));

   m_sTable.overwrite(n.varName,*pVar.release());
   m_log.s().s() << "saved to " << n.varName << std::endl;
}

void executor::visit(overlayNode& n)
{
   m_log.s().s() << "overlaying image from var " << n.varName  << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   auto& pSnip = m_sTable.demand(n.varName).as<snipSymbol>().pSnippet;

   attr.pCanvas->overlay(pSnip,n.transparent);
}

void executor::visit(removeFrameNode& n)
{
   m_log.s().s() << "removing frame" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   frameRemover::run(attr.pCanvas);
}

void executor::visit(selectObjectNode& n)
{
   m_log.s().s() << "selecting object " << std::endl;
}
