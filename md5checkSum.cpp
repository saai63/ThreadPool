#include "md5checkSum.h"
#include <openssl/md5.h>
#include <strstream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iomanip>
#include <errno.h>

bool md5checkSum::bValidate(const std::string& szFile, const std::string& szCheckSum)
{
   int n;
   MD5_CTX c;
   char buf[512];
   ssize_t bytes;
   unsigned char out[MD5_DIGEST_LENGTH];

   MD5_Init(&c);
   int fd = open(szFile.c_str(),O_RDONLY);
   if( fd == -1)
   {
      std::cout << "[#] Failed to open file " << szFile.c_str() << errno << std::endl;
      return false;
   }

   bytes=read(fd, buf, 512);
   while(bytes > 0)
   {
      MD5_Update(&c, buf, bytes);
      bytes=read(fd, buf, 512);
   }

   MD5_Final(out, &c);

   std::stringstream  strStream;
   strStream << std::hex << std::setfill('0');
   for (int idx = 0; idx < MD5_DIGEST_LENGTH; ++idx)
   {
      strStream.width(2);
      strStream << static_cast<unsigned>(out[idx]);
   }
   std::string szCalc = strStream.str();
   //std::cout << "Calculated checksum is " << szCalc.c_str() << std::endl;
   //std::cout << "Input checksum is " << szCheckSum.c_str() << std::endl;
   if( szCheckSum == szCalc)
      return true;
   else
      return false;

}
