#include "../frontend/eval.hpp"
#include "../graphics/algorithm.hpp"
#include "../graphics/snippet.hpp"
#include "executor.hpp"
#include "fileFinder.hpp"
#include "log.hpp"
#include "path.hpp"
#include "stringFileParser.hpp"
#include "symbolTable.hpp"
#include <memory>

void executor::visit(defineNode& n)
{
   auto value = argEvaluator(m_sTable,n.value).getString();
   m_log.s().s() << "defining user constant " << n.varName << std::endl;
   m_sTable.overwrite(n.varName,*new stringSymbol(value));

   visitChildren(n);
}

void executor::visit(sweepVarNode& n)
{
   iSweepableSymbol *pSymbol = iSweepableSymbol::create(
      argEvaluator(m_sTable,n.type).getString());

   argEvaluator start(m_sTable,n.start);
   argEvaluator stopOp(m_sTable,n.stopOp);
   argEvaluator stopVal(m_sTable,n.stopVal);
   argEvaluator delta(m_sTable,n.delta);

   pSymbol->start(start);

   m_sTable.overwrite(n.varName,*pSymbol);

   while(!pSymbol->isStop(stopOp,stopVal))
   {
      visitChildren(n);
      pSymbol->adjust(delta);
   }
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

void executor::visit(foreachFileNode& n)
{
   auto patt = argEvaluator(m_sTable,n.pattern).getString();
   bool allowNone = argEvaluator(m_sTable,n.allowNone).getFlag("allowNone");

   std::map<std::string,std::string> results;
   fileFinder::findAll(patt,results);
   m_log.s().s() << "found " << results.size() << " file(s)" << std::endl;
   if(results.size() == 0 && !allowNone)
      throw std::runtime_error("found no files with pattern " + patt);

   auto *pPath = new stringSymbol("");
   auto *pMatch = new stringSymbol("");
   m_sTable.overwrite(n.varName,*pPath);
   m_sTable.overwrite(n.varName+".match",*pMatch);
   for(auto it=results.begin();it!=results.end();++it)
   {
      pPath->value = it->first;
      pMatch->value = it->second;
      visitChildren(n);
   }
}

void executor::visit(ifNode& n)
{
   auto lhs = argEvaluator(m_sTable,n.lhs).getString();
   auto op  = argEvaluator(m_sTable,n.op).getString();
   auto rhs = argEvaluator(m_sTable,n.rhs).getString();

   bool ans;
   if(op == "!=")
      ans = (lhs != rhs);
   else if(op == "==")
      ans = (lhs == rhs);
   else
      throw std::runtime_error("unsupported operation for 'if'");

   if(ans)
      visitChildren(n);
}

void executor::visit(echoNode& n)
{
   m_log.s().s() << argEvaluator(m_sTable,n.text).getString() << std::endl;
   visitChildren(n);
}

void executor::visit(errorNode& n)
{
   auto text = argEvaluator(m_sTable,n.text).getString();

   if(text.empty())
      throw std::runtime_error("user requested halt");
   else
      m_errLog.s().s() << text << std::endl;

   visitChildren(n);
}

void executor::visit(accrueNode& n)
{
   auto set = argEvaluator(m_sTable,n.schema).getSet();

   if(set.size() != n.values.size())
      throw std::runtime_error("cardinality mismatch in array accrue");

   arraySymbol *pSym = (arraySymbol*)m_sTable.fetch(n.varName);
   if(pSym == NULL)
   {
      pSym = new arraySymbol();
      m_sTable.overwrite(n.varName,*pSym);
   }
   pSym->elts.push_back(std::map<std::string,iSymbol*>());
   auto& elt = pSym->elts.back();

   auto nit=set.begin();
   auto vit=n.values.begin();
   for(;nit!=set.end();++nit,++vit)
      elt[*nit] = m_sTable.demand(*vit).clone();

   visitChildren(n);
}

void executor::visit(foreachEltNode& n)
{
   auto& arr = dynamic_cast<arraySymbol&>(m_sTable.demand(n.arrayVarName));
   for(auto it=arr.elts.begin();it!=arr.elts.end();++it)
   {
      for(auto jit=it->begin();jit!=it->end();++jit)
         m_sTable.overwrite(
            n.eltVarName + "." + jit->first,
            *jit->second->clone());
      visitChildren(n);
   }
}

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
   autoReleasePtr<iFileType> pBmpFmt(attr.pApi->createFileType(iFileType::kBmp));
   attr.pImage.reset(pBmpFmt->loadBitmap(path.c_str()));
   attr.pCanvas.reset(attr.pImage.get());

   visitChildren(n);
}

void executor::visit(newImageNode& n)
{
   m_log.s().s() << "creating new image" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   // open the API
   if(attr.pApi)
      throw std::runtime_error("graphics API already in use during new");
   attr.pApi.reset(m_gFac.open(0));

   // parse args
   rect dims;
   snipSymbol *pSnip = NULL;
   if(!n.dims.empty() && n.dims.c_str()[0] == '$')
   {
      iSymbol& sym = m_sTable.demand(n.dims);
      pSnip = dynamic_cast<snipSymbol*>(&sym);
      if(pSnip)
      {
         long w,h;
         pSnip->pSnippet->getDims(w,h);
         dims = rect(0,0,w,h);
      }
   }
   if(!pSnip)
      dims = argEvaluator(m_sTable,n.dims).getRect();
   auto color = argEvaluator(m_sTable,n.color).getColor();

   // create a fresh canvas
   autoReleasePtr<iFileType> pBmpFmt(attr.pApi->createFileType(iFileType::kBmp));
   attr.pImage.reset(pBmpFmt->createNew(dims,color));
   attr.pCanvas.reset(attr.pImage.get());

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

void executor::visit(saveImageNode& n)
{
   auto path = argEvaluator(m_sTable,n.path).getString();
   m_log.s().s() << "saving image '" << path << "'" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   // pick file type
   auto destType = iFileType::kBmp;
   auto ext = getPathExtLowered(path);
   if(ext == "bmp")
      ;
   else if(ext == "png")
      destType = iFileType::kPng;
   else
      throw std::runtime_error("unknown file ext: " + ext);
   bool needsDoubleSave = (destType != iFileType::kBmp);

   autoReleasePtr<iFileType> pBmpFmt(attr.pApi->createFileType(iFileType::kBmp));
   autoDeleteFile tmpFile(createTempFilePath("cray",::rand()));

   if(needsDoubleSave)
   {
      m_log.s().s() << "  saving to " << tmpFile.path << " first..." << std::endl;
      pBmpFmt->saveBitmap(attr.pImage,tmpFile.path.c_str());
      autoReleasePtr<iGraphicsApi> pApi(m_gFac.open(1));
      autoReleasePtr<iFileTranslator> pXlator(pApi->createFileTranslator(iFileType::kBmp,destType));
      pXlator->translate(tmpFile.path.c_str(),path.c_str());
   }
   else
      pBmpFmt->saveBitmap(attr.pImage,path.c_str());

   visitChildren(n);
}

void executor::visit(snipNode& n)
{
   m_log.s().s() << "snipping image" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   std::unique_ptr<snipSymbol> pVar(new snipSymbol());

   std::unique_ptr<iTransform> pXfrm(new nullTransform);
   if(!n.xfrm.empty())
      pXfrm.reset(new rotateTransform(argEvaluator(m_sTable,n.xfrm).getReal()));

   snippetAllocator sAlloc;
   pVar->pSnippet.reset(attr.pCanvas->snip(sAlloc,*pXfrm.get()));

   auto varName = argEvaluator(m_sTable,n.varName).getString();
   m_sTable.overwrite(varName,*pVar.release());
   m_log.s().s() << "saved to " << varName << std::endl;

   visitChildren(n);
}

void executor::visit(overlayNode& n)
{
   m_log.s().s() << "overlaying image from var " << n.varName  << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   auto& pSnip = m_sTable.demand(n.varName).as<snipSymbol>().pSnippet;

   auto origin = argEvaluator(m_sTable,n.pnt).getPoint();
   autoReleasePtr<iCanvas> pCan;

   if(origin.x != 0 || origin.y != 0)
   {
      // non-zero origin, create a new canvas
      long w,h;
      attr.pCanvas->getDims(w,h);
      pCan.reset(attr.pCanvas->subset(
         rect(
            origin.x,
            origin.y,
            w - origin.x,
            h - origin.y)));
   }
   else
      // otherwise just reuse
      pCan.reset(attr.pCanvas.get());

   pCan->overlay(pSnip,argEvaluator(m_sTable,n.transparent).getColor());
   pCan.reset();

   visitChildren(n);
}

void executor::visit(surveyObjectsNode& n)
{
   m_log.s().s() << "surveying objects" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();
   auto& oattr = n.fetch<objectAttribute>();

   oattr.pObjects.reset(new objectSurvey(attr.pCanvas,m_log));
   oattr.pObjects->consumeTags(); // TODO remove this later!
   m_log.s().s() << " found " << oattr.pObjects->getNumFoundObjects() << " object(s)" << std::endl;

   visitChildren(n);
}

void executor::visit(desurveyObjectsNode& n)
{
   m_log.s().s() << "closing survey" << std::endl;

   auto& oattr = n.demandAncestor<surveyObjectsNode>().fetch<objectAttribute>();
   oattr.pObjects.reset();

   visitChildren(n);
}

void executor::visit(foreachObjectNode& n)
{
   auto& oattr = n.demandAncestor<surveyObjectsNode>().fetch<objectAttribute>();
   auto *pInt = new intSymbol();
   auto *pStr = new stringSymbol("");
   m_sTable.overwrite(n.varName,*pInt);
   m_sTable.overwrite(n.varName+".tag",*pStr);

   for(size_t i=0;i<oattr.pObjects->getNumFoundObjects();i++)
   {
      pInt->value = i;
      pStr->value = oattr.pObjects->getTag(i);
      visitChildren(n);
   }
}

void executor::visit(selectObjectNode& n)
{
   auto method = argEvaluator(m_sTable,n.method).getString();
   auto arg = argEvaluator(m_sTable,n.arg).getString();
   m_log.s().s() << "selecting object " << method << ", " << arg << std::endl;

   auto& attr = n.root().fetch<graphicsAttribute>();
   auto& oattr = n.demandAncestor<surveyObjectsNode>().fetch<objectAttribute>();

   if(method == "idx")
   {
      if(n.arg == "/")
      {
         rect r = oattr.pObjects->superset();
         attr.pCanvas.reset(attr.pCanvas->subset(r));
      }
      else
      {
         rect r = oattr.pObjects->findObject(argEvaluator(m_sTable,n.arg).getInt());
         attr.pCanvas.reset(attr.pCanvas->subset(r));
      }
   }
   else if(method == "tag")
   {
      rect r = oattr.pObjects->findObjectByTag(argEvaluator(m_sTable,n.arg).getString());
      attr.pCanvas.reset(attr.pCanvas->subset(r));
   }
   else
      throw std::runtime_error("unsupported selection mode: " + method);

   visitChildren(n);
}

void executor::visit(selectObjectNodeOLD& n)
{
   m_log.s().s() << "selecting object " << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   objectSurvey os(attr.pCanvas,m_log);
   m_log.s().s() << " found " << os.getNumFoundObjects() << " object(s)" << std::endl;

   bool withTags = argEvaluator(m_sTable,n.withTags).getFlag("withTags");
   if(withTags)
   {
      os.consumeTags();
      auto tag = os.getTag(argEvaluator(m_sTable,n.n).getInt());
      m_log.s().s() << "tag is '" << tag << "'" << std::endl;
   }

   if(argEvaluator(m_sTable,n.hilight).getFlag("hilight"))
   {
      for(size_t i=0;i<os.getNumFoundObjects();i++)
      {
         rect r = os.findObject(i);
         drawBox(r,RGB(0,255,0),attr.pCanvas);
      }
   }

   rect r = os.findObject(argEvaluator(m_sTable,n.n).getInt());
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

void executor::visit(getDimsNode& n)
{
   m_log.s().s() << "reading dims" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   long w,h;
   if(!n.obj.empty())
   {
      auto& pSnip = m_sTable.demand(n.obj).as<snipSymbol>().pSnippet;
      pSnip->getDims(w,h);
   }
   else
      attr.pCanvas->getDims(w,h);

   auto varName = argEvaluator(m_sTable,n.varName).getString();
   m_sTable.overwrite(varName,*new stringSymbol(argEvaluator::fmtRect(rect(0,0,w,h))));

   visitChildren(n);
}

void executor::visit(boxNode& n)
{
   auto& attr = n.root().fetch<graphicsAttribute>();

   COLORREF outl = RGB(0,255,0);
   rect r;
   COLORREF fill = 0xFFFFFFFF;

   if(!n.outlineCol.empty())
      outl = argEvaluator(m_sTable,n.outlineCol).getColor();
   if(n.rect.empty())
   {
      long w,h;
      attr.pCanvas->getDims(w,h);
      r = rect(0,0,w,h);
   }
   else
      r = argEvaluator(m_sTable,n.rect).getRect();
   if(!n.fillCol.empty())
      fill = argEvaluator(m_sTable,n.fillCol).getColor();

   if(fill == 0xFFFFFFFF)
      drawBox(r,outl,attr.pCanvas);
   else
      drawBox(r,outl,fill,attr.pCanvas);
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

   // restore the copy at (0,0)
   // use an invalid color so the overlay ignores transparency and unconditionally writes
   attr.pImage->overlay(pSnip,0xFFFFFFFF);

   // resize image
   attr.pImage->setDims(sw,sh);
   attr.pCanvas.reset(attr.pImage.get());

   visitChildren(n);
}

void executor::visit(selectFontNode& n)
{
   std::string face;
   size_t pnt;
   argEvaluator(m_sTable,n.fnt).getFont(face,pnt);

   m_log.s().s() << "creating font '" << face << "':" << pnt << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   COLORREF color = 0xFFFFFFFF;
   std::list<std::string> options = n.options;
   auto maybeColor = argEvaluator(m_sTable,n.color).getString();
   if(maybeColor.empty())
      ; // ignore it
   else if(::strncmp(maybeColor.c_str(),"rgb{",4)==0)
      color = argEvaluator(m_sTable,n.color).getColor();
   else
      options.push_front(n.color);

   std::map<std::string,size_t> table;
   table["italic"]    = iFont::kItalic;
   table["underline"] = iFont::kUnderline;
   table["strikeout"] = iFont::kStrikeout;
   table["opaquebk"]  = iFont::kOpaqueBackground;
   table["bold"]      = iFont::kBold;
   size_t flags = argEvaluator::computeBitFlags(m_sTable,options,table);

   attr.pFont.reset(attr.pApi->createFont(face.c_str(),pnt,color,flags));

   visitChildren(n);
}

void executor::visit(deselectFontNode& n)
{
   auto& attr = n.root().fetch<graphicsAttribute>();
   attr.pFont.reset();
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

void executor::visit(surveyFrameNode& n)
{
   m_log.s().s() << "surveying frame" << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();
   auto& fattr = n.fetch<frameAttribute>();

   fattr.pFramer.reset(new framer(attr.pCanvas));
   if(n.color.empty())
      fattr.pFramer->inferFrameColorFromOrigin();
   else
      fattr.pFramer->initFrameColor(argEvaluator(m_sTable,n.color).getColor());

   fattr.pFramer->findFrame();

   visitChildren(n);
}

void executor::visit(desurveyFrameNode& n)
{
   m_log.s().s() << "closing frame survey" << std::endl;
   auto& fattr = n.fetch<frameAttribute>();

   fattr.pFramer.reset();

   visitChildren(n);
}

void executor::visit(fillNode& n)
{
   m_log.s().s() << "filling" << std::endl;
   auto& fattr = n.demandAncestor<surveyFrameNode>().fetch<frameAttribute>();

   fattr.pFramer->colorFrame(argEvaluator(m_sTable,n.color).getColor());

   visitChildren(n);
}

void executor::visit(tightenNode& n)
{
   m_log.s().s() << "tighening frame (this could take a while)" << std::endl;

   // build criteria
   std::unique_ptr<iPixelCriteria> pCri;
   auto cri = argEvaluator(m_sTable,n.method).getString();
   if(cri == "min-lightness")
   {
      double threshold = argEvaluator(m_sTable,n.arg).getReal();
      pCri.reset(new lightnessPixelCriteria(threshold));
   }
   else
      throw std::runtime_error("unknown tighten method");

   auto& fattr = n.demandAncestor<surveyFrameNode>().fetch<frameAttribute>();
   COLORREF col;
   if(n.color.empty())
      col = fattr.pFramer->getFrameColor();
   else
      col = argEvaluator(m_sTable,n.color).getColor();

   auto& attr = n.root().fetch<graphicsAttribute>();
   outliner o(attr.pCanvas,*fattr.pFramer.get(),m_log);
   o.encroach(*pCri.get(),col);

   visitChildren(n);
}

void executor::visit(loosenNode& n)
{
   m_log.s().s() << "loosening frame (this could take a while)" << std::endl;

   auto& attr = n.root().fetch<graphicsAttribute>();
   auto& fattr = n.demandAncestor<surveyFrameNode>().fetch<frameAttribute>();
   outliner o(attr.pCanvas,*fattr.pFramer.get(),m_log);
   o.retreat(argEvaluator(m_sTable,n.color).getColor());

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

void executor::visit(desurveyWhiskersNode& n)
{
   m_log.s().s() << "closing whisker survey" << std::endl;
   auto& wattr = n.demandAncestor<surveyWhiskersNode>().fetch<whiskerAttribute>();

   wattr.pSurvey.reset();

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

void executor::visit(nudgeNode& n)
{
   auto mode = argEvaluator(m_sTable,n.mode).getString();
   auto varName = argEvaluator(m_sTable,n.varName).getString();

   if(mode == "left-in")
   {
      auto r = argEvaluator(m_sTable,n.in).getRect();
      auto amt = argEvaluator(m_sTable,n.amt).getPixelCount();
      r.x += amt;
      r.w -= amt;
      m_sTable.overwrite(varName,*new stringSymbol(argEvaluator::fmtRect(r)));
   }
   else if(mode == "up")
   {
      auto pt = argEvaluator(m_sTable,n.in).getPoint();
      auto amt = argEvaluator(m_sTable,n.amt).getPixelCount();
      pt.y -= amt;
      m_sTable.overwrite(varName,*new stringSymbol(argEvaluator::fmtPoint(pt)));
   }
   else if(mode == "right")
   {
      auto pt = argEvaluator(m_sTable,n.in).getPoint();
      auto amt = argEvaluator(m_sTable,n.amt).getPixelCount();
      pt.x += amt;
      m_sTable.overwrite(varName,*new stringSymbol(argEvaluator::fmtPoint(pt)));
   }
   else if(mode == "up-by-half")
   {
      auto pt = argEvaluator(m_sTable,n.in).getPoint();
      auto& pSnip = m_sTable.demand(n.amt).as<snipSymbol>().pSnippet;
      long w,h;
      pSnip->getDims(w,h);
      long up = h / 2.0;
      pt.y -= up;
      m_sTable.overwrite(varName,*new stringSymbol(argEvaluator::fmtPoint(pt)));
   }
   else if(mode == "right-by-width")
   {
      auto pt = argEvaluator(m_sTable,n.in).getPoint();
      auto& pSnip = m_sTable.demand(n.amt).as<snipSymbol>().pSnippet;
      long w,h;
      pSnip->getDims(w,h);
      pt.x += w;
      m_sTable.overwrite(varName,*new stringSymbol(argEvaluator::fmtPoint(pt)));
   }
   else
      throw std::runtime_error("unknown nudge mode: " + mode);

   visitChildren(n);
}

void executor::visit(pixelTransformNode& n)
{
   auto op = argEvaluator(m_sTable,n.op).getString();

   std::unique_ptr<iPixelCriteria> pCri;
   std::unique_ptr<iPixelTransform> pXfrm;
   if(op == "red-shift")
      pXfrm.reset(new componentShift('r',argEvaluator(m_sTable,n.arg).getInt()));
   else if(op == "blue-shift")
      pXfrm.reset(new componentShift('b',argEvaluator(m_sTable,n.arg).getInt()));
   else if(op == "green-shift")
      pXfrm.reset(new componentShift('g',argEvaluator(m_sTable,n.arg).getInt()));
   else if(op == "lightness-shift")
      pXfrm.reset(new lightnessShift(argEvaluator(m_sTable,n.arg).getInt()));
   else if(op == "to-mono")
   {
      pCri.reset(new lightnessPixelCriteria(argEvaluator(m_sTable,n.arg).getReal()));
      pXfrm.reset(new toMonochromeShift(*pCri.get()));
   }
   else
      throw std::runtime_error("unknown transform");

   m_log.s().s() << "running pixel transform " << op << " " << std::endl;
   auto& attr = n.root().fetch<graphicsAttribute>();

   pixelTransformer pt(attr.pCanvas,m_log);
   pt.run(*pXfrm.get());

   visitChildren(n);
}

void executor::visit(writeTagNode& n)
{
   auto text = argEvaluator(m_sTable,n.text).getString();
   m_log.s().s() << "generating tag '" << text << "'" << std::endl;

   auto& attr = n.root().fetch<graphicsAttribute>();
   auto tagEnd = tagWriter(attr.pCanvas,m_log).write(text);

   // write human text underneath
   rect textBox(0,tagEnd+2,102,50);
   rect textBnd(3,tagEnd+2,101,49);
   drawBox(textBox,RGB(0,0,0),RGB(255,174,201),attr.pCanvas);
   {
      autoReleasePtr<iFont> pFnt(attr.pApi->createFont("system",8,RGB(0,0,0),0));

      size_t flags = 0;
      flags |= DT_NOPREFIX;
      flags |= DT_TOP;
      flags |= DT_WORDBREAK;
      flags |= DT_END_ELLIPSIS;
      attr.pCanvas->drawText(
         textBnd,
         text.c_str(),
         flags,
         pFnt);
   }

   visitChildren(n);
}

void executor::visit(readTagNode& n)
{
   auto& attr = n.root().fetch<graphicsAttribute>();

   auto value = tagReader(attr.pCanvas,m_log).readIf();
   m_log.s().s() << "read tag '" << value << "'" << std::endl;

   m_sTable.overwrite(n.varName,*new stringSymbol(value));

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
