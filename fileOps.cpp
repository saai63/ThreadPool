#include "fileOps.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <list>

#define CHUNK_SIZE 1024 // 10 MB

//int open(const char *pathname, int flags);


bool fileOps::bCopyFile(const std::string& src, const std::string& dst)
{
   int fd_src = open(src.c_str(), O_RDONLY);
   if(fd_src == -1)
   {
      std::cout << "Failed to open file " << src.c_str() << " with error : " << strerror(errno)  << std::endl;
      return false;
   }

   std::string dest = dst + "/" + src;
   int fd_dst = open(dest.c_str(), O_RDWR | O_CREAT, 0777);
   if(fd_dst == -1)
   {
      std::list<std::string> folder;
      if(errno == 2)
      {
         char* data1 = (char*)dest.c_str();
         char *remainder = strtok(data1, "/");
         while(remainder != NULL)
         {
            folder.push_back(remainder);
            //std::cout << remainder << std::endl;
            remainder = strtok(NULL,"/");
         }
         folder.pop_back();
         std::string folder_to_create;

         std::list<std::string>::iterator it = folder.begin();
         for( ; it != folder.end(); it++)
         {
            if(!folder_to_create.empty())
               folder_to_create = folder_to_create + "/" + (*it);
            else
               folder_to_create = (*it);
            //std::cout << folder_to_create.c_str() << std::endl;
            int ret = mkdir(folder_to_create.c_str(), 0777);
            if( ret == 0)
               continue;
            else if( (ret != 0) && (errno == EEXIST))
            {
               continue;
            }
            else
            {
               return false;
            }
         }
         fd_dst = open((dst + "/" +src).c_str(), O_RDWR | O_CREAT, 0777);
         if(fd_dst == -1)
         {
            std::cout << "[*]Failed to open file " << dest.c_str() << " with error : " << strerror(errno)  << std::endl;
            return false;
         }
      }
   }

   void *data = malloc(CHUNK_SIZE);
   if(!data)
   {
      std::cout << "Failed to allocate memory!" << std::endl;
      return false;
   }

   int count =CHUNK_SIZE;
   int read_count = -1;
   while(1)
   {
      memset(data, 0, CHUNK_SIZE);
      read_count = read(fd_src, data, count);
      if(read_count <= 0)
         break;
      write(fd_dst, data, count);      
   }
   //free(data);
   close(fd_dst);
   close(fd_src);

   return true;
}

