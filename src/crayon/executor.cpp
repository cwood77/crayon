#include "executor.hpp"
#include "log.hpp"

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

   attr.pImage.reset();
   attr.pApi.reset();

   visitChildren(n);
}
