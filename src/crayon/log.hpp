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
   explicit log(iLog& l) : m_log(l), m_n(0) {}

   tmpLog s() { return tmpLog(*this); }

   void write(const std::string& msg) { m_log.write(msg.c_str()); }

private:
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

std::ostream& operator<<(std::ostream& s, const indent& i);

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

class coutLog : public iLog {
public:
   virtual void write(const char *msg);
};
