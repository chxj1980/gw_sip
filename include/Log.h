

#ifndef __LOG__
#define __LOG__

#include <stdio.h>

static FILE * pFile = fopen("/opt/app/log/log.txt","a");
#define LOG(format, ...) \
   if (pFile) {\
    fprintf(pFile, "[%s@%s,%d,%ld] " format "\n", \
             __func__, __FILE__, __LINE__, time(NULL), ##__VA_ARGS__ ); \
    fflush(pFile);}
#endif