#include "../frontend/eval.hpp"
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

   visitChildren(n);
}

void executor::visit(overlayNode& n)
{
   m_log.s().s() << "overlaying image from var " << n.varName  << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   auto& pSnip = m_sTable.demand(n.varName).as<snipSymbol>().pSnippet;
   long w,h;
   pSnip->getDims(w,h);

   auto origin = argEvaluator(m_sTable,n.pnt).getPoint();

   autoReleasePtr<iCanvas> pSubCan(attr.pCanvas->subset(rect(origin.x,origin.y,w,h)));
   pSubCan->overlay(pSnip,argEvaluator(m_sTable,n.transparent).getColor());
   pSubCan.reset();

   visitChildren(n);
}

void executor::visit(removeFrameNode& n)
{
   m_log.s().s() << "removing frame" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   frameRemover::run(attr.pCanvas);

   visitChildren(n);
}

void executor::visit(selectObjectNode& n)
{
   m_log.s().s() << "selecting object " << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   rect r = objectFinder::run(
      attr.pCanvas,
      argEvaluator(m_sTable,n.n).getNum(),
      argEvaluator(m_sTable,n.hilight).getFlag("hilight"),
      m_log);
   attr.pCanvas.reset(attr.pCanvas->subset(r));

   visitChildren(n);
}

void executor::visit(deselectObjectNode& n)
{
   m_log.s().s() << "deselecting object " << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   attr.pCanvas.reset(attr.pCanvas->superset());

   visitChildren(n);
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
         attr.pImage->setPixel(point(x,y),RGB(255,255,255));

   // restore the copy at (0,0)
   attr.pImage->overlay(pSnip,RGB(255,255,255));

   // resize image
   attr.pImage->setDims(sw,sh);
   attr.pCanvas.reset(attr.pImage.get());

   visitChildren(n);
}

void executor::visit(defineNode& n)
{
   m_log.s().s() << "defining user constant " << n.varName << std::endl;
   m_sTable.overwrite(n.varName,*new stringSymbol(n.value));

   visitChildren(n);
}

void executor::visit(findWhiskersNode& n)
{
   m_log.s().s() << "finding whiskers" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   COLORREF xColor = whiskerFinder::kCenter;
   if(argEvaluator(m_sTable,n.x).getString() != "/")
      xColor = argEvaluator(m_sTable,n.x).getColor();
   COLORREF yColor = whiskerFinder::kCenter;
   if(argEvaluator(m_sTable,n.y).getString() != "/")
      yColor = argEvaluator(m_sTable,n.y).getColor();

   auto pnt = whiskerFinder::run(attr.pCanvas,xColor,yColor,m_log);
   m_log.s().s() << "  whisker found at (" << pnt.x << "," << pnt.y << ")" << std::endl;

   std::stringstream varBody;
   varBody << "pnt{" << pnt.x << "," << pnt.y << "}";
   m_sTable.overwrite(n.varName,*new stringSymbol(varBody.str()));

   visitChildren(n);
}

void executor::visit(trimWhiskersNode& n)
{
   m_log.s().s() << "triming whiskers" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   whiskerFinder::clear(attr.pCanvas,m_log);

   visitChildren(n);
}

void executor::visit(foreachStringSetNode& n)
{
   auto path = argEvaluator(m_sTable,n.filePath).getString();
   auto schema = argEvaluator(m_sTable,n.schema).getSet();

   m_log.s().s() << "pulling strings from '" << path << "'" << std::endl;
   m_log.s().s() << "{" << std::endl;
   for(auto x : schema)
      m_log.s().s() << "   " << x << std::endl;
   m_log.s().s() << "}" << std::endl;

   visitChildren(n);
}

void executor::visit(closeStringSetNode& n)
{
   m_log.s().s() << "closing stringset" << std::endl;
   visitChildren(n);
}

void executor::visit(echoNode& n)
{
   m_log.s().s() << argEvaluator(m_sTable,n.text).getString() << std::endl;
   visitChildren(n);
}
