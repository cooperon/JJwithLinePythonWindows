#define WINDOWS
#include <time.h>
#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include "TimeFuncs.h"

#ifdef WINDOWS
static LARGE_INTEGER lpFrequency;
static LARGE_INTEGER initialCount; 
#else
typedef struct timeval TIME_STRUCT;
static void GetTime(TIME_STRUCT *timeStruct);
static TIME_STRUCT initialTime;
#endif


void InitializeTimeMeasure(void)
{
#ifdef WINDOWS	
 QueryPerformanceFrequency(&lpFrequency);
 QueryPerformanceCounter(&initialCount);
#else
 GetTime(&initialTime);
#endif
}

#ifndef WINDOWS	
void GetTime(TIME_STRUCT *timeStruct)
{
 struct timezone timeZone;
 gettimeofday(timeStruct,&timeZone);
}
#endif

double GetRunTime(void)
{
#ifdef WINDOWS
 LARGE_INTEGER lpPerformanceCount;
 QueryPerformanceCounter(&lpPerformanceCount);
 return  (double)(lpPerformanceCount.QuadPart - initialCount.QuadPart) / (double)lpFrequency.QuadPart;
#else
 struct timeval currentTime;
 struct timezone timeZone;
 gettimeofday(&currentTime,&timeZone);
 return (double)(currentTime.tv_sec-initialTime.tv_sec)+(currentTime.tv_usec-initialTime.tv_usec)*1.e-6;
#endif
}

char* GetDateTime(void)
{
 time_t timeval;
 (void)time(&timeval);
 return ctime(&timeval);
}
