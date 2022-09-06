#include "../crayon/log.hpp"
#include "../graphics/graphicsApi.hpp"
#include <stdexcept>

class api : iGraphicsApi {
public:
   virtual void addref() {}
   virtual void release() {}
   virtual iFileType *createFileType(size_t i) { return NULL; }
   virtual iFont *createFont(const char *face, size_t size) { return NULL; }
};

__declspec(dllexport) iGraphicsApi *create(iLog& l)
{
   throw std::runtime_error("unsupported");
}
