#include "../crayon/test.hpp"
#include "../graphics/graphicsApi.hpp"
#include "path.hpp"
#include <cmath>

#ifdef cdwTestBuild

cdwTest(getPathDirIf_basic)
{
   cdwAssertEqu("dir",getPathDirIf("dir\\file.txt"));
   cdwAssertEqu("dir\\dir2",getPathDirIf("dir\\dir2\\file.txt"));
   cdwAssertEqu("",getPathDirIf("file.txt"));
}

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

cdwTest(polar_to_cart)
{
   polPoint polar;
   polar.r = 3.162278;
   polar.theta = -1.892547;
   auto xy = polar.toCartesian();

   cdwAssertEqu(-1,xy.x);
   cdwAssertEqu(-3,xy.y);
}

cdwTest(cart_to_polar)
{
   point p(3,-1);
   auto polar = polPoint::fromCartesian(p);
   cdwAssertEqu(polar.r,3.162278);
   cdwAssertEqu(polar.theta,-0.321751);

   auto backagain = polar.toCartesian();
   cdwAssertEqu(3,backagain.x);
   cdwAssertEqu(-1,backagain.y);
}

#endif // cdwTestBuild
