#include "../frontend/ast.hpp"
#include "../frontend/dumpVisitor.hpp"
#include "../frontend/lexor.hpp"
#include "../frontend/parser.hpp"
#include "test.hpp"
#include <iostream>
#include <memory>
#include <sstream>

int main(int,const char *[])
{
   basicAsserter ba;
   testBase::runAll(ba);

   return 0;
}

#if 0
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <stdio.h>
#include <typeinfo>

void dumpTriple(char *_pThumb, size_t pix)
{
   unsigned char *pThumb = (unsigned char*)(_pThumb + (3*pix));
   size_t b = pThumb[0];
   size_t g = pThumb[1];
   size_t r = pThumb[2];
   ::printf("pixel %llu is (RGB) = (%llu,%llu,%llu)\n",pix,r,g,b);

   double R = r;
   double G = g;
   double B = b;

   auto lum = (0.2126*R + 0.7152*G + 0.0722*B);
   ::printf("lum1 = %llu\n",(size_t)lum);

   lum = (0.299*R + 0.587*G + 0.114*B);
   ::printf("lum2 = %llu\n",(size_t)lum);
}

template<class T>
DWORD tryDumpInfoHeader(char *pBlock, long& h)
{
   auto *pInfoHdr = (T*)(pBlock + sizeof(BITMAPFILEHEADER));
   ::printf("\n");
   if(pInfoHdr->biSize == sizeof(T))
      ::printf("match! - %s\n",typeid(T).name());
   else
   {
      ::printf("no match for %s; %lu != %llu\n",typeid(T).name(),pInfoHdr->biSize,sizeof(T));
      return 0;
   }

   ::printf("biWidth = %ld\n",pInfoHdr->biWidth);
   ::printf("biHeight = %ld\n",pInfoHdr->biHeight);
   ::printf("biPlanes = %d\n",pInfoHdr->biPlanes);
   ::printf("biBitCount = %hd\n",pInfoHdr->biBitCount);
   ::printf("biCompression = %ld\n",pInfoHdr->biCompression);
   ::printf("biSizeImage = %ld\n",pInfoHdr->biSizeImage);
   ::printf("biXPelsPerMeter = %ld\n",pInfoHdr->biXPelsPerMeter);
   ::printf("biYPelsPerMeter = %ld\n",pInfoHdr->biYPelsPerMeter);
   ::printf("biClrUsed = %ld\n",pInfoHdr->biClrUsed);
   ::printf("biClrImportant = %ld\n",pInfoHdr->biClrImportant);
   ::printf("sizeof(info header) = %llu\n",sizeof(T));

   h = pInfoHdr->biHeight;
   return pInfoHdr->biSizeImage;
}

int main(int,const char *[])
{
   const char  *pInPath = "C:\\Users\\cwood\\Desktop\\game15 - dogwater\\templates done\\wind sketch.bmp";
   const char *pOutPath = "C:\\Users\\cwood\\Desktop\\game15 - dogwater\\templates done\\wind sketch - out.bmp";

   FILE *fp = ::fopen(pInPath,"rb");
   ::printf("file handle is %llu\n",(size_t)fp);

   ::fseek(fp,0,SEEK_END);
   long size = ::ftell(fp);
   ::fseek(fp,0,SEEK_SET);
   ::printf("size is %ld\n",size);

   auto *pBlock = new char[size];
   ::printf("read %llu byte(s)\n",::fread(pBlock,size,1,fp));
   ::fclose(fp);

   auto *pFileHdr = (BITMAPFILEHEADER*)pBlock;
   ::printf("\n");
   ::printf("bfSize = %lu\n",pFileHdr->bfSize);
   ::printf("bfOffBits = %lu\n",pFileHdr->bfOffBits);
   ::printf("sizeof(header) = %llu\n",sizeof(BITMAPFILEHEADER));

   long imageHeight = 0;
   auto imageSize = tryDumpInfoHeader<BITMAPINFOHEADER>(pBlock,imageHeight);
   //tryDumpInfoHeader<BITMAPV4HEADER>(pBlock); .... really windows? :(
   //tryDumpInfoHeader<BITMAPV5HEADER>(pBlock);

   dumpTriple(pBlock + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), 0);
   dumpTriple(pBlock + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), 1);
   dumpTriple(pBlock + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), 2);

   // now with GDI

   auto dc = ::CreateCompatibleDC(NULL);
   ::printf("dc = %llu\n",(size_t)dc);

   auto bit = (HBITMAP)::LoadImageA(NULL, pInPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   ::printf("hbitmap = %llu\n",(size_t)bit);
   ::SelectObject(dc,bit);

   // make a change
   ::printf("bkmode is %d\n",::GetBkMode(dc));
   ::SetBkMode(dc,TRANSPARENT);
   RECT r;
   r.left = 0;
   r.right = 10000;
   r.bottom = 10000;
   ::printf("drawText returned %d\n",::DrawText(dc,"Hello World",-1,&r,0));

   // save from GDI
   auto *pNewBlock = new char[imageSize];
   auto success = ::GetDIBits(dc,bit,0,imageHeight,pNewBlock,(BITMAPINFO*)(pBlock + sizeof(BITMAPFILEHEADER)),DIB_RGB_COLORS);
   ::printf("GetDIBits returned %d\n",success);
   auto *pOutFile = ::fopen(pOutPath,"wb");
   ::fwrite(pFileHdr,sizeof(BITMAPFILEHEADER),1,pOutFile);
   ::fwrite(pBlock + sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFOHEADER),1,pOutFile);
   ::fwrite(pNewBlock,imageSize,1,pOutFile);
   ::fclose(pOutFile);

   delete [] pNewBlock;
   delete [] pBlock;
   ::DeleteObject(bit);
   ::DeleteDC(dc);

   return 0;
}
#endif
