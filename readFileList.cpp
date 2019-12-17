#include "readFileList.h"

FileList::FileList(std::string checkSumFile)
{
   m_fp = fopen(checkSumFile.c_str(), "r");
   if(!m_fp)
   {
      std::cout << "Failed to open checksum file!" << std::endl;
      m_bValid =  false;
   }
   else
      m_bValid =  true;
}

bool FileList::bIsValid()
{
   return m_bValid;
}

std::vector<sFileEntry> FileList::lSerialize()
{
   std::vector<sFileEntry> pVec;
   char *md5sum = new char[1000];
   char *fileName = new char[1000];
   while(!feof(m_fp))
   {
      fscanf(m_fp,"%s %s",md5sum, fileName);
      sFileEntry dummy;
      dummy.szMd5sum = md5sum;
      dummy.szFileName = fileName;
      pVec.push_back(dummy);
   }
   fclose(m_fp);
   delete md5sum;
   delete fileName;
   m_bValid = false;
   return pVec;
}

