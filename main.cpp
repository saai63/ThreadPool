#include <vector>
#include <iostream>
#include "readFileList.h"
#include "fileOps.h"
#include "md5checkSum.h"
#include "threadpool.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
   if(argc != 2)
   {
      std::cout << "Usage ./fileCopy <number of threads>" << std::endl;
      return -1;
   }
   int numThreads = atoi(argv[1]);
   FileList *pFiles = new FileList("TestFiles/md5sum.txt");
   if(!pFiles->bIsValid())
   {
      std::cout << "Failed to create FileList obj!" << std::endl;
      return -1;
   }
   std::vector<sFileEntry> entries = pFiles->lSerialize();
   std::vector<sFileEntry>::iterator itr = entries.begin();

#ifdef CURRENT_CODE
   for( ;itr!=entries.end(); itr++)
   {
      //std::cout << "The entry is " << (*itr).szFileName << "\t\t\t" << (*itr).szMd5sum << std::endl;
      std::string src = "TestFiles/" + (*itr).szFileName;
      std::string dst = "Dst";
      fileOps::bCopyFile(src, dst); 
      if(!md5checkSum::bValidate(dst+"/"+src, (*itr).szMd5sum))
         std::cout << "Checksum verificaton failed for " << (*itr).szFileName << std::endl;
   }
#endif

#ifdef THREADPOOL
   ThreadPool *tp = new ThreadPool(numThreads);
   for( ;itr!=entries.end(); itr++)
   {
      //std::cout << "The entry is " << (*itr).szFileName << "\t\t\t" << (*itr).szMd5sum << std::endl;
      std::string src = "TestFiles/" + (*itr).szFileName;
      std::string dst = "Dst";
      tp->addTask(new copyFiles(src, dst, (*itr).szMd5sum));
   }
   delete tp;
#endif
   return 0;
}
