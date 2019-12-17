#ifndef __MDSCHECKSUM_H__
#define __MDSCHECKSUM_H__
#include <iostream>
#include <string>

class md5checkSum{
public:
   static bool bValidate(const std::string& szFile, const std::string& szCheckSum);
};
#endif
