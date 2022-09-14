#include "../frontend/ast.hpp"
#include "../frontend/crawler.hpp"
#include "../frontend/lexor.hpp"
#include "../frontend/parser.hpp"
#include "../graphics/graphicsApi.hpp"
#include "cfile.hpp"
#include "executor.hpp"
#include "log.hpp"
#include "symbolTable.hpp"
#include "test.hpp"
#include <iostream>
#include <memory>

int main(int argc, const char *argv[])
{
   try
   {
      coutLog lSink;
      log Log(lSink);

      if(argc == (1+1) && argv[1] == std::string("diag"))
      {
         // run diagnostics
         graphicsApiFactory graf(lSink);
         graf.diagnostics();
      }
      else if(argc == (1+1) && argv[1] == std::string("test"))
      {
         // run tests
         basicAsserter ba;
         testBase::runAll(ba);
      }
      else if(argc == (1+1))
      {
         // run script

         // setup root
         graphicsApiFactory graf(lSink);
         attributeStore attrs;
         std::unique_ptr<scriptNode> pRoot(new scriptNode());
         attributeStoreBinding _asb(*pRoot.get(),attrs);

         // find all scripts
         std::list<std::string> allScriptPaths;
         crawler::crawl(argv[1],allScriptPaths,Log);
         allScriptPaths.push_back(argv[1]);

         for(auto scriptPath : allScriptPaths)
         {
            Log.s().s() << "parsing " << scriptPath << std::endl;

            // read file
            cFileBlock blk;
            inCFile::readAllContents(scriptPath,blk);

            // parse
            {
               lexor l(blk.pBlock);
               parser p(l,scriptPath,*pRoot.get());
               p.parseFile();
            }
         }

         // execute
         symbolTable sTable;
         executor xfrm(Log,graf,sTable);
         pRoot->acceptVisitor(xfrm);

         // teardown
         graf.markSuccess();

         Log.s().s() << "leaving" << std::endl;
      }
      else
         throw std::runtime_error("usage: crayon.exe <script> or crayon.exe 'test'");
   }
   catch(std::exception& x)
   {
      std::cerr << "ERROR: " << x.what() << std::endl;
      return -1;
   }
   return 0;
}
