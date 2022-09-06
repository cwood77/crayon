#include "cfile.hpp"
#include <stdexcept>

void inCFile::readAllContents(const std::string& path, cFileBlock& b)
{
   inCFile f(path);
   f.seek(2);
   auto s = f.tell();
   f.seek(0);
   f.read(s,b);
}

inCFile::inCFile(const std::string& path)
: m_fp(NULL)
{
   m_fp = ::fopen(path.c_str(),"rb");
   if(!m_fp)
      throw std::runtime_error("failed to open file for read");
}

inCFile::~inCFile()
{
   ::fclose(m_fp);
}

void inCFile::seek(size_t i)
{
   int orig;
   switch(i)
   {
      case 0:
         orig = SEEK_SET;
         break;
      case 1:
         orig = SEEK_CUR;
         break;
      case 2:
         orig = SEEK_END;
         break;
   }
   ::fseek(m_fp,0,orig);
}

size_t inCFile::tell()
{
   return ::ftell(m_fp);
}

void inCFile::read(size_t n, cFileBlock& blk)
{
   delete blk.pBlock;
   blk.pBlock = new char [n+1];
   blk.pBlock[n] = 0;

   ::fread(blk.pBlock,n,1,m_fp);
}

outCFile::outCFile(const std::string& path)
{
   m_fp = ::fopen(path.c_str(),"wb");
   if(!m_fp)
      throw std::runtime_error("failed to open file for write");
}

outCFile::~outCFile()
{
   ::fclose(m_fp);
}

void outCFile::writeBytes(const void *pPtr, size_t n)
{
   ::fwrite(pPtr,n,1,m_fp);
}
