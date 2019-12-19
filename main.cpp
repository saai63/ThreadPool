#include <vector>
#include <iostream>
#include "readFileList.h"
#include "fileOps.h"
#include "md5checkSum.h"
#include "threadpool.h"
#include <stdlib.h>
#include "msgThread.h"

#ifdef PARALLEL_CHECKSUM
void sendMsg(std::string data)
{
   static int qid = -1;
   if( qid == -1)
   {
      qid = msgget(MSG_PORTID, IPC_CREAT | 0666);
      if( qid == -1)
      {
         std::cout << "Failed to create msg queue!" << std::endl;
         return;
      }
   }
   struct Mymsgbuf buf;
   buf.mtype = 1;
   memset(buf.mtext, 0, 580);
   strcpy(buf.mtext,(char*)data.c_str());
   if (msgsnd(qid, (void *) &buf, sizeof(buf.mtext),
         IPC_NOWAIT) == -1) {
      perror("msgsnd error");
      return;
   }
   //printf("sent: %s\n", msg.mtext);
}
#endif

int main(int argc, char* argv[])
{
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
   if(argc != 2)
   {
      std::cout << "Usage ./fileCopy <number of threads>" << std::endl;
      return -1;
   }
   int numThreads = atoi(argv[1]);
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
#ifdef PARALLEL_CHECKSUM
   msgThread::spawnCheckSumThread();  // Start the thread!
   for( ;itr!=entries.end(); itr++)
   {
      //std::cout << "The entry is " << (*itr).szFileName << "\t\t\t" << (*itr).szMd5sum << std::endl;
      std::string src = "TestFiles/" + (*itr).szFileName;
      std::string dst = "Dst";
      fileOps::bCopyFile(src, dst); 
      //if(!md5checkSum::bValidate(dst+"/"+src, (*itr).szMd5sum))
      //   std::cout << "Checksum verificaton failed for " << (*itr).szFileName << std::endl;
      std::string packedNameChecksum = dst+"/"+src+";"+(*itr).szMd5sum;
      sendMsg(packedNameChecksum);
   }
   sendMsg("EXIT;EXIT");
   msgThread::waitForThreadExit();
#endif
   return 0;
}
