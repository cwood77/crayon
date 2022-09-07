#include "../crayon/cfile.hpp"
#include "api.hpp"

iBitmap *bmpFileType::loadBitmap(const char *path)
{
   autoReleasePtr<bitmap> pBitmap(new bitmap(Api));

   // load file directly to check modes, etc., and stash dims
   {
      cFileBlock pBlock;
      inCFile::readAllContents(path,pBlock);

      auto *pFileHeader = (BITMAPFILEHEADER*)pBlock.pBlock;
      checkCompatible(*pFileHeader);

      auto *pInfoHeader = (BITMAPINFOHEADER*)(pBlock.pBlock + sizeof(BITMAPFILEHEADER));
      checkCompatible(*pInfoHeader);

      pBitmap->width = pInfoHeader->biWidth;
      pBitmap->height = pInfoHeader->biHeight;
   }

   // load file _again_, this time using GDI
   auto bit = (HBITMAP)::LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   Api.Log.s().s() << "loaded hbitmap at " << (size_t)bit << std::endl;
   if(!bit)
      throw std::runtime_error("failed to load bitmap");
   ::SelectObject(Api.dc,bit);

   return pBitmap.leak();
}

void bmpFileType::saveBitmap(iBitmap& _b, const char *path)
{
   auto& b = dynamic_cast<bitmap&>(_b);
   size_t imageSize = calculateMagicSize(b.width,b.height);

   BITMAPFILEHEADER fHdr;
   BITMAPINFO iHdrEtc;
   populateStructs(b.width,b.height,fHdr,iHdrEtc.bmiHeader);

   auto *pBlock = new char[imageSize];
   auto hBitmap = (HBITMAP)::GetCurrentObject(Api.dc,OBJ_BITMAP);
   auto success = ::GetDIBits(
      Api.dc,
      hBitmap,
      0,        // start
      b.height, // #lines
      pBlock,
      &iHdrEtc,
      DIB_RGB_COLORS);
   if(!success)
      throw std::runtime_error("failed to acquire bits from GDI");
   checkCompatible(iHdrEtc.bmiHeader);

   outCFile f(path);
   f.writeType(fHdr);
   f.writeType(iHdrEtc.bmiHeader);
   f.writeBytes(pBlock,imageSize);
   delete [] pBlock;
}

void bmpFileType::populateStructs(long w, long h, BITMAPFILEHEADER& fHdr, BITMAPINFOHEADER& iHdr)
{
   fHdr.bfType = 19778;
   fHdr.bfSize = calculateMagicSize(w,h) + 14 + 40;
   fHdr.bfReserved1 = 0;
   fHdr.bfReserved2 = 0;
   fHdr.bfOffBits = 14 + 40;
   checkCompatible(fHdr);

   iHdr.biSize = 40;
   iHdr.biWidth = w;
   iHdr.biHeight = h;
   iHdr.biPlanes = 1;
   iHdr.biBitCount = 24;
   iHdr.biCompression = 0;
   iHdr.biSizeImage = calculateMagicSize(w,h);
   iHdr.biXPelsPerMeter = 0;
   iHdr.biYPelsPerMeter = 0;
   iHdr.biClrUsed = 0;
   iHdr.biClrImportant = 0;
   checkCompatible(iHdr);
}

#define cdwRequireField(__field__,__value__) \
   if(hdr.__field__ != __value__) \
   { \
      Api.Log.s().s() << "expected " << __value__ << " but got " << hdr.__field__ << std::endl; \
      throw std::runtime_error("unsupported field value for " #__field__); \
   }

void bmpFileType::checkCompatible(BITMAPFILEHEADER& hdr)
{
   //::printf("bfType = %d\n",hdr.bfType);
   //::printf("bfSize = %lu\n",hdr.bfSize); // literal file size
   //::printf("bfOffBits = %lu\n",hdr.bfOffBits);
   //::printf("sizeof(header) = %llu\n",sizeof(BITMAPFILEHEADER));

   cdwRequireField(bfType,19778);
   cdwRequireField(bfReserved1,0);
   cdwRequireField(bfReserved2,0);
   cdwRequireField(bfOffBits,(14 + 40));
}

void bmpFileType::checkCompatible(BITMAPINFOHEADER& hdr)
{
   cdwRequireField(biSize,40);
   cdwRequireField(biPlanes,1);
   cdwRequireField(biBitCount,24);
   cdwRequireField(biCompression,0);
   cdwRequireField(biXPelsPerMeter,0);
   cdwRequireField(biYPelsPerMeter,0);
   cdwRequireField(biClrUsed,0);
   cdwRequireField(biClrImportant,0);

   cdwRequireField(biSizeImage,calculateMagicSize(hdr.biWidth,hdr.biHeight));

   //::printf("biSize = %ld\n",hdr.biSize);
   //::printf("biWidth = %ld\n",hdr.biWidth);
   //::printf("biHeight = %ld\n",hdr.biHeight);
   //::printf("biPlanes = %d\n",hdr.biPlanes);
   //::printf("biBitCount = %hd\n",hdr.biBitCount);
   //::printf("biCompression = %ld\n",hdr.biCompression);
   //::printf("biSizeImage = %ld\n",hdr.biSizeImage); // file size - 54
   //::printf("biXPelsPerMeter = %ld\n",hdr.biXPelsPerMeter);
   //::printf("biYPelsPerMeter = %ld\n",hdr.biYPelsPerMeter);
   //::printf("biClrUsed = %ld\n",hdr.biClrUsed);
   //::printf("biClrImportant = %ld\n",hdr.biClrImportant);
}

DWORD bmpFileType::calculateMagicSize(long w, long h)
{
   // for performance reasons, BMPs have a width that is DWORD-aligned
   unsigned long stride = 3 * w; // 3 for 24-bits (i.e. 3 bytes)
   unsigned long strideAligned = stride;
   if(strideAligned & 0x3)
      strideAligned = (stride + 0x4) & ~0x3;
   return strideAligned * h;
}
