#include "../frontend/eval.hpp"
#include "../graphics/algorithm.hpp"
#include "../graphics/snippet.hpp"
#include "executor.hpp"
#include "log.hpp"
#include "stringFileParser.hpp"
#include "symbolTable.hpp"
#include <memory>

void executor::visit(loadImageNode& n)
{
   auto path = argEvaluator(m_sTable,n.path).getString();
   m_log.s().s() << "loading image '" << path << "'" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   // open the API
   if(attr.pApi)
      throw std::runtime_error("graphics API already in use during load");
   attr.pApi.reset(m_gFac.open(0));

   // load the file
   autoReleasePtr<iFileType> pBmpFmt(attr.pApi->createFileType(0));
   attr.pImage.reset(pBmpFmt->loadBitmap(path.c_str()));
   attr.pCanvas.reset(attr.pImage.get());

   visitChildren(n);
}

void executor::visit(saveImageNode& n)
{
   auto path = argEvaluator(m_sTable,n.path).getString();
   m_log.s().s() << "saving image '" << path << "'" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   autoReleasePtr<iFileType> pBmpFmt(attr.pApi->createFileType(0));
   pBmpFmt->saveBitmap(attr.pImage,path.c_str());

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
   auto value = argEvaluator(m_sTable,n.value).getString();
   m_log.s().s() << "defining user constant " << n.varName << std::endl;
   m_sTable.overwrite(n.varName,*new stringSymbol(value));

   visitChildren(n);
}

void executor::visit(surveyWhiskersNode& n)
{
   m_log.s().s() << "finding whiskers" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();
   auto& wattr = n.fetch<whiskerAttribute>();

   wattr.pSurvey.reset(new whiskerSurvey(attr.pCanvas,m_log));

   visitChildren(n);
}

void executor::visit(findWhiskerPointNode& n)
{
   m_log.s().s() << "finding whiskers" << std::endl;
   auto& wattr = n.demandAncestor<surveyWhiskersNode>().fetch<whiskerAttribute>();

   COLORREF xColor = whiskerSurvey::kCenter;
   if(argEvaluator(m_sTable,n.x).getString() != "/")
      xColor = argEvaluator(m_sTable,n.x).getColor();
   COLORREF yColor = whiskerSurvey::kCenter;
   if(argEvaluator(m_sTable,n.y).getString() != "/")
      yColor = argEvaluator(m_sTable,n.y).getColor();

   auto pnt = wattr.pSurvey->findPoint(xColor,yColor);
   m_log.s().s() << "  whisker found at (" << pnt.x << "," << pnt.y << ")" << std::endl;

   std::stringstream varBody;
   varBody << "pnt{" << pnt.x << "," << pnt.y << "}";
   m_sTable.overwrite(n.varName,*new stringSymbol(varBody.str()));

   visitChildren(n);
}

void executor::visit(trimWhiskersNode& n)
{
   m_log.s().s() << "triming whiskers" << std::endl;
   auto& wattr = n.demandAncestor<surveyWhiskersNode>().fetch<whiskerAttribute>();

   wattr.pSurvey->clear();

   visitChildren(n);
}

void executor::visit(desurveyWhiskersNode& n)
{
   m_log.s().s() << "closing whisker survey" << std::endl;
   auto& wattr = n.demandAncestor<surveyWhiskersNode>().fetch<whiskerAttribute>();

   wattr.pSurvey.reset();

   visitChildren(n);
}

void executor::visit(foreachStringSetNode& n)
{
   auto path = argEvaluator(m_sTable,n.filePath).getString();
   auto schema = argEvaluator(m_sTable,n.schema).getSet();

   m_log.s().s() << "pulling strings from '" << path << "'" << std::endl;

   std::list<std::list<std::string> > stringSet;
   stringFileParser::parse(path,schema,stringSet);
   m_log.s().s() << "found " << stringSet.size() << " tuple(s)" << std::endl;

   for(auto tuple : stringSet)
   {
      auto sit = schema.begin();
      auto var = tuple.begin();
      for(;sit!=schema.end();++sit,++var)
      {
         std::string fullName = n.varName + "." + *sit;
         m_sTable.overwrite(fullName,*new stringSymbol(trimTrailingNewlines(*var)));
      }
      visitChildren(n);
   }
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

void executor::visit(drawTextNode& n)
{
   m_log.s().s() << "drawing text" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   if(!attr.pFont)
      throw std::runtime_error("a font must be active to draw text");

   const bool isRectMode =
      (::strncmp(argEvaluator(m_sTable,n.pt).getString().c_str(),"rect",4)==0);

   std::map<std::string,size_t> table;
   table["hCenter"] = DT_CENTER;
   table["hLeft"]   = DT_LEFT;
   /*
   table["hRight"]  = DT_RIGHT;
   table["vTop"]    = DT_TOP;         // these are not allowed, b/c they affect
   table["vBottom"] = DT_BOTTOM;      // rect calculation inside graphics APIs
   table["vCenter"] = DT_VCENTER;
   */
   size_t flags = argEvaluator::computeBitFlags(m_sTable,n.options,table);
   flags |= DT_NOPREFIX;
   flags |= DT_NOCLIP;

   if(isRectMode)
   {
      // rect mode

      flags |= DT_TOP;
      flags |= DT_WORDBREAK;

      attr.pCanvas->drawText(
         argEvaluator(m_sTable,n.pt).getRect(),
         argEvaluator(m_sTable,n.text).getString().c_str(),
         flags,
         attr.pFont);
   }
   else
   {
      // point mode

      flags |= DT_BOTTOM;
      flags |= DT_SINGLELINE;

      attr.pCanvas->drawText(
         argEvaluator(m_sTable,n.pt).getPoint(),
         argEvaluator(m_sTable,n.text).getString().c_str(),
         flags,
         attr.pFont);
   }

   visitChildren(n);
}

std::string executor::trimTrailingNewlines(const std::string& s)
{
   if(s.length() == 0)
      return s;

   const char *pThumb = s.c_str()+s.length()-1;
   for(;pThumb>s.c_str()&&(*pThumb=='\r'||*pThumb=='\n');--pThumb);

   return std::string(s.c_str(),pThumb-s.c_str()+1);
}

void executor::visit(selectFontNode& n)
{
   std::string face;
   size_t pnt;
   argEvaluator(m_sTable,n.fnt).getFont(face,pnt);

   m_log.s().s() << "creating font '" << face << "':" << pnt << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   std::map<std::string,size_t> table;
   table["italic"]    = iFont::kItalic;
   table["underline"] = iFont::kUnderline;
   table["strikeout"] = iFont::kStrikeout;
   table["opaquebk"]  = iFont::kOpaqueBackground;
   table["bold"]      = iFont::kBold;
   size_t flags = argEvaluator::computeBitFlags(m_sTable,n.options,table);

   attr.pFont.reset(attr.pApi->createFont(face.c_str(),pnt,flags));

   visitChildren(n);
}

void executor::visit(deselectFontNode& n)
{
   auto& attr = n.root().fetch<graphicsAttribute>();
   attr.pFont.reset();
   visitChildren(n);
}
