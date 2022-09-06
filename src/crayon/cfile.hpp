#pragma once
#include <string>

class cFileBlock {
public:
   cFileBlock() : pBlock(NULL) {}
   ~cFileBlock() { delete[] pBlock; }

   char *pBlock;
};

class inCFile {
public:
   static void readAllContents(const std::string& path, cFileBlock& b);

   explicit inCFile(const std::string& path);
   ~inCFile();

   void seek(size_t i);
   size_t tell();

   void read(size_t n, cFileBlock& blk);

private:
   FILE *m_fp;
};

class outCFile {
public:
   explicit outCFile(const std::string& path);
   ~outCFile();

   template<class T>
   void writeType(const T& block) { writeBytes(&block,sizeof(T)); }

   void writeBytes(const void *pPtr, size_t n);

private:
   FILE *m_fp;
};
