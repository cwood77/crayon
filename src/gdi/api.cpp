#include "../crayon/log.hpp"
#include "../graphics/graphicsApi.hpp"
#include <stdexcept>

class api : public iGraphicsApi {
public:
   virtual iFileType *createFileType(size_t i) { return NULL; }

   virtual iFont *createFont(const char *face, size_t size)
   { throw std::runtime_error("font not yet supported"); }

cdwImplAddrefRelease();
};

__declspec(dllexport) iGraphicsApi *create(iLog& l)
{
   throw std::runtime_error("unsupported");
}
