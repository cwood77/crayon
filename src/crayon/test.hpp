#pragma once
#ifdef cdwTestBuild
#include <set>
#include <sstream>
#include <string>

class testBase;

class iAsserter {
public:
   virtual void require(const testBase& t, size_t line, const std::string& expected, const std::string& actual) = 0;
};

class basicAsserter : public iAsserter {
public:
   virtual void require(const testBase& t, size_t line, const std::string& expected, const std::string& actual);
};

class testBase {
public:
   static void runAll(iAsserter& a);

   virtual ~testBase();
   virtual const char *getName() const = 0;
   virtual void run(iAsserter& a) const = 0;

protected:
   testBase();

private:
   static std::set<testBase*>& getAll();
};

#define cdwTest(__name__) \
class __name__##Test : public testBase { \
public: \
   virtual const char *getName() const { return #__name__; } \
   virtual void run(iAsserter& _a) const; \
} g##__name__##Inst; \
void __name__##Test::run(iAsserter& _a) const \

#define cdwAssertEqu(__expected__,__actual__) \
   _a.require(*this,__LINE__,_asserter::toString(__expected__),_asserter::toString(__actual__));

namespace _asserter {

template<class T>
inline std::string toString(const T& v)
{
   std::stringstream stream;
   stream << v;
   return stream.str();
}

} // namespace _asserter

#endif // cdwTestBuild
