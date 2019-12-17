#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>

typedef struct entry{
   std::string szMd5sum;
   std::string szFileName;
} sFileEntry;

class FileList{
public:
   FileList(std::string checkSumFile);
   bool bIsValid();
   std::vector<sFileEntry> lSerialize();
private:
   FILE *m_fp;
   bool m_bValid;
};
