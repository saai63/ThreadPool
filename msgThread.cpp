#include "msgThread.h"
#include "md5checkSum.h"


void* msgRcvThread(void* arg)
{
   int qid = msgget(MSG_PORTID, IPC_CREAT | 0666);
   if(qid == -1)
   {
      std::cout << "Failed to create/open the msg queue with errno : " << errno << std::endl;
      return NULL;
   }
   int msgtype = 1;
   while(1)
   {
      struct Mymsgbuf msg;
      if (msgrcv(qid, (void *) &msg, sizeof(msg.mtext), msgtype, MSG_NOERROR | IPC_NOWAIT) == -1)
      {
         if (errno != ENOMSG) 
         {
            perror("msgrcv");
            return NULL;
         }
      } 
      else
      {
         //printf("message received: %s\n", msg.mtext);
         std::string msgRcvd = msg.mtext;
         if(msgRcvd == "EXIT;EXIT")
         {
            std::cout << "Received exit msg from main thread" << std::endl;
            return NULL;
         }
         else
         {
            std::string fName = msgRcvd.substr(0,msgRcvd.find(";"));
            std::string checkSum = msgRcvd.substr(msgRcvd.find(";") + 1);
            if(!md5checkSum::bValidate(fName, checkSum))
               std::cout << "Checksum validation failed for " << fName.c_str() << " [" << checkSum.c_str() << "] " << std::endl;
         }
      }
   }
}

void msgThread::spawnCheckSumThread()
{
   if(pthread_create(&handle, NULL,msgRcvThread, NULL))
   {
      std::cout << "Failed to create msg thread!" << std::endl;
         return;
   }
}

void msgThread::waitForThreadExit()
{
   pthread_join(handle, NULL);
}
