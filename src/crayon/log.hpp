#pragma once
#include <sstream>
#include <string>

class log;

class tmpLog {
public:
   explicit tmpLog(log& l) : m_log(l) {}
   ~tmpLog();

   std::ostream& s() { return m_s; }

private:
   log& m_log;
   std::stringstream m_s;
};

class iLog {
public:
   virtual void write(const char *msg) = 0;
};

class log {
public:
   explicit log(iLog& l, const std::string& prefix = "")
   : m_prefix(prefix), m_log(l), m_n(0) {}

   tmpLog s() { return tmpLog(*this); }

   void write(const std::string& msg) { m_log.write((m_prefix + msg).c_str()); }

private:
   const std::string m_prefix;
   iLog& m_log;
   size_t m_n;

friend class autoIndent;
friend class indent;
};

class indent {
public:
   explicit indent(log& l) : m_l(l) {}

   size_t getN() const { return m_l.m_n; }
   log& m_l;
};

inline std::ostream& operator<<(std::ostream& s, const indent& i)
{
   s << std::string(i.getN(),' ');
   return s;
}

class autoIndent {
public:
   explicit autoIndent(log& l) : m_l(l) { m_l.m_n+=3; }
   ~autoIndent() { m_l.m_n-=3; }

private:
   log& m_l;
};

class bufferLog : public iLog {
public:
   virtual void write(const char *msg) { buffer << msg; }

   std::stringstream buffer;
};

class ostreamLog : public iLog {
public:
   explicit ostreamLog(std::ostream& sink) : m_sink(sink) {}

   virtual void write(const char *msg) { m_sink << msg; }

private:
   std::ostream& m_sink;
};

inline tmpLog::~tmpLog()
{
   m_log.write(m_s.str());
}
