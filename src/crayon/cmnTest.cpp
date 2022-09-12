#include "../crayon/test.hpp"
#include "path.hpp"

#ifdef cdwTestBuild

cdwTest(getPathExt_happy)
{
   cdwAssertEqu("bar",getPathExtLowered("foo.bar"));
}

cdwTest(getPathExt_lower)
{
   cdwAssertEqu("bar",getPathExtLowered("foo.bAR"));
}

cdwTest(getPathExt_muti)
{
   cdwAssertEqu("png",getPathExtLowered("foo.bmp.png"));
}

#endif // cdwTestBuild
