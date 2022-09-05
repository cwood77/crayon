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

namespace _test {

class testLog : public iLog {
public:
   virtual void write(const char *msg) { v << msg; }

   std::stringstream v;
};

} // namespace _test

cdwTest(log_acceptance)
{
   _test::testLog sink;
   log l(sink);
   l.s().s() << "foo";

   cdwAssertEqu("foo",sink.v.str());
}

#endif // cdwTestBuild
