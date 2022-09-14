#include "log.hpp"
#include "test.hpp"
#include <iostream>

#ifdef cdwTestBuild

cdwTest(log_acceptance)
{
   bufferLog sink;
   log l(sink);
   l.s().s() << "foo";

   cdwAssertEqu("foo",sink.buffer.str());
}

#endif // cdwTestBuild
