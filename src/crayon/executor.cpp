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
   auto& attr = n.root().fetch<graphicsAttribute>();

   rect r = objectFinder::run(attr.pCanvas,n.n,n.dbgHilight,m_log);
   attr.pCanvas.reset(attr.pCanvas->subset(r));
}

void executor::visit(cropNode& n)
{
   m_log.s().s() << "cropping" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   // stash dims
   long sw,sh;
   attr.pCanvas->getDims(sw,sh);
   m_log.s().s() << "selection dims {" << sw << "," << sh << "}" << std::endl;

   // make a copy
   autoReleasePtr<iSnippet> pSnip;
   snippetAllocator sAlloc;
   nullTransform nullXfrm;
   pSnip.reset(attr.pCanvas->snip(sAlloc,nullXfrm));

   // clear all the canvas(es)
   attr.pCanvas.reset();

   // clear the entire image
   long w,h;
   attr.pImage->getDims(w,h);
   m_log.s().s() << "clearing canvas {" << w << "," << h << "}" << std::endl;
   for(long x=0;x<w;x++)
      for(long y=0;y<h;y++)
         attr.pImage->setPixel(point(x,y),RGB(0,0,255));

   // restore the copy at (0,0)
   attr.pImage->overlay(pSnip,RGB(255,255,255));

   // resize image
   attr.pImage->setDims(sw,sh);
   attr.pCanvas.reset(attr.pImage.get());
}
