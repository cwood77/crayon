#include "log.hpp"
#include "test.hpp"
#include <iostream>

tmpLog::~tmpLog()
{
   m_log.write(m_s.str());
}

std::ostream& operator<<(std::ostream& s, const indent& i)
{
   s << std::string(i.getN(),' ');
   return s;
}

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
