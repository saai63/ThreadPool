#ifndef __FILEOPS_H__
#define __FILEOPS_H__
#include <string.h>
#include <iostream>

class fileOps{
public:
   static bool bCopyFile(const std::string& src, const std::string& dst);
};
#endif
