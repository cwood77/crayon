#include "log.hpp"
#include "test.hpp"
#include <iostream>

void coutLog::write(const char *msg)
{
   std::cout << msg;
}

#ifdef cdwTestBuild

cdwTest(log_acceptance)
{
   bufferLog sink;
   log l(sink);
   l.s().s() << "foo";

   cdwAssertEqu("foo",sink.buffer.str());
}

#endif // cdwTestBuild
