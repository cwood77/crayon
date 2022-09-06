#include "api.hpp"

iBitmap *bmpFileType::loadBitmap(const char *path)
{
   auto bit = (HBITMAP)::LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   Api.Log.s().s() << "loaded bitmap at " << (size_t)bit << std::endl;
   if(!bit)
      throw std::runtime_error("failed to load bitmap");
   ::SelectObject(Api.dc,bit);
   return new bitmap(Api);
}

void bmpFileType::saveBitmap(iBitmap& b, const char *path)
{
   throw std::runtime_error("unimpled 3");
}
