

#include <stdio.h>
#include <stdarg.h>
#include "../include/SNLogger.h"
#include <time.h>

int globalDebug = TRUE;

void SNLogger(unsigned long ulErrorType, int iLine, const char *szFile, const char *pszFormat, ...){
    static int iCallCounter = 0;
    static FILE *fLogFile;
    if (fLogFile == NULL){
        fLogFile = fopen("debug.txt", "a");
        time_t theTime = time(NULL);
        struct tm tm = *localtime(&theTime);
        fprintf(fLogFile, "\nTime: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    }

    va_list vaArgumentPointer;
    char szOutputString[256] = {0};
    const char *pszType;

    switch (ulErrorType) {
        case 0:
            pszType = "Error";
        break;
        case 1:
            pszType = "Debug";
        break;
        case 2:
            pszType = "Info";
        break;
        default:
            pszType = "Unknown";
        break;
    }

    va_start(vaArgumentPointer, pszFormat);
    vsnprintf(szOutputString, 256-1, pszFormat, vaArgumentPointer);
    va_end(vaArgumentPointer);

    fprintf(fLogFile, "%04i: %s:%s Path: %s:%i\n", iCallCounter, pszType, szOutputString, szFile, iLine);
    fflush(fLogFile);
    iCallCounter++;
}