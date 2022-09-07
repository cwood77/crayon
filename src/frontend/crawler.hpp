#pragma once
#include <string>
#include <list>

class log;

class crawler {
public:
   static void crawl(const std::string& scriptPath, std::list<std::string>& addtlScripts, log& Log);
};
