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

void bmpFileType::saveBitmap(iBitmap& b, const char *path)
{
   throw std::runtime_error("unimpled 3");
}

void bmpFileType::populateStructs(long w, long h, BITMAPFILEHEADER& fHdr, BITMAPINFOHEADER& iHdr)
{
}

#define cdwRequireField(__field__,__value__) \
   if(hdr.__field__ != __value__) \
      throw std::runtime_error("unsupported field value for " #__field__);

void bmpFileType::checkCompatible(BITMAPFILEHEADER& hdr)
{
   //::printf("bfType = %d\n",hdr.bfType);
   //::printf("bfSize = %lu\n",hdr.bfSize); // literal file size
   //::printf("bfOffBits = %lu\n",hdr.bfOffBits);
   //::printf("sizeof(header) = %llu\n",sizeof(BITMAPFILEHEADER));

   cdwRequireField(bfType,19778);
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

   // 24-bit BMP means 3 bytes per pixel
   cdwRequireField(biSizeImage,(3ul * hdr.biWidth * hdr.biHeight));

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
