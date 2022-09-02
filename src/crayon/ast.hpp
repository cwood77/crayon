#pragma once
#include <string>

class scriptNode {
};

class loadImageNode : public scriptNode {
public:
   std::string path;
};

class saveImageNode : public scriptNode {
public:
   std::string path;
};
