#include "../frontend/ast.hpp"
#include "../frontend/crawler.hpp"
#include "../frontend/dumpVisitor.hpp"
#include "../frontend/lexor.hpp"
#include "../frontend/parser.hpp"
#include "../graphics/graphicsApi.hpp"
#include "cfile.hpp"
#include "executor.hpp"
#include "imageCache.hpp"
#include "log.hpp"
#include "symbolTable.hpp"
#include "test.hpp"
#include <iostream>
#include <memory>

int main(int argc, const char *argv[])
{
   try
   {
      ostreamLog lSink(std::cout);
      log Log(lSink);
      ostreamLog lErrSink(std::cerr);
      log errLog(lErrSink);

      if(argc == (1+1) && argv[1] == std::string("diag"))
      {
         // run diagnostics
         graphicsApiFactory graf(lSink);
         graf.diagnostics();
      }
      else if(argc == (1+1) && argv[1] == std::string("test"))
      {
#ifdef cdwTestBuild
         // run tests
         basicAsserter ba;
         testBase::runAll(ba);
#else
         Log.s().s() << "tests compiled out" << std::endl;
#endif
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
            try
            {
               lexor l(blk.pBlock);
               parser p(l,scriptPath,*pRoot.get());
               p.parseFile();
            }
            catch(std::exception& x)
            {
               Log.s().s() << "dumping parse output {{" << std::endl;
               dumpVisitor v(Log);
               pRoot->acceptVisitor(v);
               Log.s().s() << "}}" << std::endl;
               throw;
            }
         }

         // optimize
         symbolTable sTable;
         deferredImageCalculator defCalc(Log,errLog,graf,sTable);
         imageCacheFinder optimizer(defCalc,Log);
         pRoot->acceptVisitor(optimizer);

         // execute
         executor exec(Log,errLog,graf,sTable);
         pRoot->acceptVisitor(exec);

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
