#ifdef cdwTestBuild
#include "test.hpp"
#include <iostream>
#include <stdexcept>

void basicAsserter::require(const testBase& t, size_t line, const std::string& expected, const std::string& actual)
{
   if(expected != actual)
   {
      std::cerr << "test failed: " << t.getName() << ":" << line << std::endl;
      std::cerr << "   expected: '" << expected << "'" << std::endl;
      std::cerr << "   actual: '" << actual << "'" << std::endl;
      throw std::runtime_error("test failed");
   }
}

void testBase::runAll(iAsserter& a)
{
   std::cout << "running tests" << std::endl;
   for(auto *pTest : getAll())
   {
      std::cout << "   " << pTest->getName() << std::endl;
      pTest->run(a);
   }
   std::cout << "done" << std::endl;
}

testBase::~testBase()
{
   getAll().erase(this);
}

testBase::testBase()
{
   getAll().insert(this);
}

std::set<testBase*>& testBase::getAll()
{
   static std::set<testBase*> _a;
   return _a;
}

#endif // cdwTestBuild
