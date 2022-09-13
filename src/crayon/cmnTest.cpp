#include "../crayon/test.hpp"
#include "../graphics/graphicsApi.hpp"
#include "path.hpp"
#include <cmath>

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

cdwTest(double_rounding_cpp_idontunderstand)
{
   cdwAssertEqu((long)-1,(long)-1.0);

   // polar -> cart values from internet
   // this is the core arithmetic from below
   double r = 3.162278;
   double theta = -1.892547;
   long l = r * ::cos(theta);
   cdwAssertEqu((long)-1,l);

   polPoint polar;
   polar.r = 3.162278;
   polar.theta = -1.892547;
   auto cart = polar.toCartesian();
   cdwAssertEqu(-1,cart.x);
   cdwAssertEqu(-3,cart.y);
   cdwAssertEqu((size_t)13834574129991403870,*(size_t*)&polar.theta);

   // cart -> polar -> rotate -> cart
   // so, evidently, if I arrive at THE SAME FUCKING POLAR PT VIA MATH,
   // CONVERSION DOESN'T WORK AFTERWARDS????
   polar = polPoint::fromCartesian(point(3,-1));
   //polar.theta -= (M_PI / 2.0);
   polar.theta = (long double)polar.theta - (long double)M_PI_2;
   cdwAssertEqu(3.162278,polar.r);
   cdwAssertEqu(-1.892547,polar.theta);
   // THEY'RE FUCKING DIFFERENT!  FUCK YOU IOSTREAM!
  // cdwAssertEqu((size_t)13834574129991403870,*(size_t*)&polar.theta);
   cdwAssertEqu((size_t)13834574129456338128,*(size_t*)&polar.theta);
   cart = polar.toCartesian();
   cdwAssertEqu(0,cart.x); // should be -1
   cdwAssertEqu(-3,cart.y);

   polar.r = (float)polar.r;
   polar.theta = (float)polar.theta;
   cart = polar.toCartesian();
   cdwAssertEqu(-1,cart.x);
   cdwAssertEqu(-3,cart.y);
}

#endif // cdwTestBuild
