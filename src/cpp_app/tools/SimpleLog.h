#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#ifdef ANDROID
#include "AndroidBacktrace.h"
#else
inline void backtraceToLogcat()
{
}
#endif

#define SIMPLE_LOG_FORMAT_EXTRA_ARGS " %s:%d"
#ifndef ANDROID
#define SIMPLE_LOG_FORMAT_TIME_PREFIX "[%.6f] "
#else
#define SIMPLE_LOG_FORMAT_TIME_PREFIX "%s"
#endif

#define SIMPLE_LOG_LAST_AT_MOST_N_CHARS(string, n) (string + (strlen(string) <= n ? 0 : strlen(string) - n) )
#define SIMPLE_LOG_EXTRA_ARGS SIMPLE_LOG_LAST_AT_MOST_N_CHARS(__FILE__,40),__LINE__

#define LOG(...) LOGI(__VA_ARGS__)
#define LOGC(...) { SIMPLE_LOG_PRINTC(out, __VA_ARGS__, "");}
#define LOGRET(...) { SIMPLE_LOG_PRINTF(out, "\r", SIMPLE_LOG_FORMAT_TIME_PREFIX __VA_ARGS__,SIMPLE_LOG_EXTRA_ARGS);}
#define LOGI(...)   { SIMPLE_LOG_PRINTF(out, "\n", SIMPLE_LOG_FORMAT_TIME_PREFIX __VA_ARGS__,SIMPLE_LOG_EXTRA_ARGS);}
#define LOGW(...)   { SIMPLE_LOG_PRINTF(err, "\n", SIMPLE_LOG_FORMAT_TIME_PREFIX "WARNING: " __VA_ARGS__,__FILE__,__LINE__); }
#define LOGE(...)   { SIMPLE_LOG_PRINTF(err, "\n", SIMPLE_LOG_FORMAT_TIME_PREFIX "ERROR: " __VA_ARGS__,__FILE__,__LINE__); backtraceToLogcat(); throw std::runtime_error("SIMPLE LOG ERROR/ASSERT"); }
#define LOGN(...) snprintf(NULL, 0, __VA_ARGS__)
#define ASSERT(condition, ...) if( !(condition) )  { SIMPLE_LOG_PRINTC(err, "\n%s", "ASSERT(" #condition ")\n", ""); LOGE(__VA_ARGS__); }
#define ASSERTS(condition, string) ASSERT(condition, "%s", (string).c_str())

#define USE(x) {while(typeid(x).hash_code() == typeid(x).hash_code() + 1);}


extern FILE* SIMPLE_LOG_STD_FILE;
#define LOG_INIT_STD_FILE(fileName) {ASSERT(SIMPLE_LOG_STD_FILE == NULL, "Log std file was already inited"); SIMPLE_LOG_STD_FILE = fopen(fileName, "w+"); \
    ASSERT(SIMPLE_LOG_STD_FILE != NULL);}


//#ifdef ANDROID
    #define SIMPLE_LOG_STDERR_TO_FILE_REDIRECT(file) int ret = dup2(fileno(file), fileno(stderr)); if(ret == -1) LOGE("Couldn't redirect stderr to file");
//#else
//    #define SIMPLE_LOG_STDERR_TO_FILE_REDIRECT(file)
//#endif

extern FILE* SIMPLE_LOG_ERR_FILE;
#define LOG_INIT_ERR_FILE(fileName) { \
    ASSERT(SIMPLE_LOG_ERR_FILE == NULL, "Log err file was already inited"); \
    SIMPLE_LOG_ERR_FILE = fopen(fileName, "w+"); \
    ASSERT(SIMPLE_LOG_ERR_FILE != NULL, "Couln't open file %s to save logs in it", fileName); \
    SIMPLE_LOG_STDERR_TO_FILE_REDIRECT(SIMPLE_LOG_ERR_FILE); \
    }



#include <time.h>


extern uint64_t ApplicationStartTimeNanoSec;

static inline uint64_t getApplicationTimeNanoSec()
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    uint64_t time = static_cast<uint64_t>(tp.tv_sec) * 1000000000 + tp.tv_nsec;
    if (ApplicationStartTimeNanoSec == 0)
    {
        ApplicationStartTimeNanoSec = time;
        return getApplicationTimeNanoSec();
    }
    return time - ApplicationStartTimeNanoSec;
}

#ifndef ANDROID
    #define SIMPLE_LOG_GET_TIME (getApplicationTimeNanoSec() / 1e9)
#else
    #define SIMPLE_LOG_GET_TIME ""
#endif




/// Return time elapsed since application started (actually this function called first time)
/// Microseconds (1e-6) us
static inline uint64_t getApplicationTime()
{
    return getApplicationTimeNanoSec() / 1000;
}



#define SIMPLE_LOG_PRINTC(outtype, format , ...) SIMPLE_LOG_GENERIC_PRINT(outtype, format "%s\n", __VA_ARGS__)
#define SIMPLE_LOG_PRINTF(outtype, new_line_type, format, ...) SIMPLE_LOG_GENERIC_PRINT(outtype, format SIMPLE_LOG_FORMAT_EXTRA_ARGS new_line_type, SIMPLE_LOG_GET_TIME, __VA_ARGS__)

#define SIMPLE_LOG_GENERIC_PRINT(outtype, ...) SIMPLE_LOG_FILE_PRINT(outtype, __VA_ARGS__); SIMPLE_LOG_CONSOLE_PRINT(outtype, __VA_ARGS__)


#define SIMPLE_LOG_FILE_PRINT(outtype, ...) if(SIMPLE_LOG_STD_FILE) {fprintf(SIMPLE_LOG_STD_FILE, __VA_ARGS__); fflush(SIMPLE_LOG_STD_FILE);} \
    if( SIMPLE_LOG_ERR_FILE && strcmp(#outtype, "err") == 0 ) {fprintf(SIMPLE_LOG_ERR_FILE, __VA_ARGS__); fflush(SIMPLE_LOG_ERR_FILE);}


#ifndef ANDROID
    #define SIMPLE_LOG_CONSOLE_PRINT(outtype, ...) fprintf(std##outtype, __VA_ARGS__); fflush(std##outtype)
#else
    #include <android/log.h>
    #define SIMPLE_LOG_ANDROID_LOG_TAG "geocv"
    #define SIMPLE_LOG_ANDROID_LOG_TYPE(outtype) (strcmp(#outtype, "out") == 0 ? ANDROID_LOG_INFO : ANDROID_LOG_ERROR)
    #define SIMPLE_LOG_CONSOLE_PRINT(outtype, ...) __android_log_print(SIMPLE_LOG_ANDROID_LOG_TYPE(outtype), SIMPLE_LOG_ANDROID_LOG_TAG, __VA_ARGS__)
#endif



#endif // SIMPLE_LOG_H

























