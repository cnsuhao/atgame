#include "atgBase.h"
#include <time.h>

#ifdef _WIN32
    #include <Windows.h>
#endif // _WIN32

#ifdef _ANDROID
    #include <android/log.h>
    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
    #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#endif

char atgStaticLogBuf[1024];

extern void atgLog( const char* format, ... )
{
    va_list argptr;
    va_start(argptr, format);
    
    static const char* dateFormat = "%02d-%02d-%02d %02d:%02d:%02d\t";
    //> ÉèÖÃÊ±¼ä
    time_t timer = time(NULL);
    tm t = *localtime(&timer);

#define DATA_PARAMETERS  (t.tm_year+1900)%100, t.tm_mon+1, t.tm_mday,t.tm_hour, t.tm_min, t.tm_sec

    int sz;
    sz = fprintf(stderr, dateFormat, DATA_PARAMETERS);
    sz += vfprintf(stderr, format, argptr);
#if defined _WIN32
    if (sz > 0)
    {
        if (sz >= 1024)
        {
            char* buf = new char[sz + 1];
            int n = sprintf(atgStaticLogBuf, dateFormat, DATA_PARAMETERS);
            vsprintf(buf + n, format, argptr);
            buf[sz] = 0;
            OutputDebugStringA(buf);
            SAFE_DELETE_ARRAY(buf);
        }else
        {
            int n = sprintf(atgStaticLogBuf, dateFormat, DATA_PARAMETERS);
            vsprintf(atgStaticLogBuf + n, format, argptr);
            atgStaticLogBuf[sz] = 0;
            OutputDebugStringA(atgStaticLogBuf);
        }
    }
#elif defined _ANDROID
    if (sz > 0)
    {
        if (sz >= 1024)
        {
            char* buf = new char[sz + 1];
            int n = sprintf(atgStaticLogBuf, dateFormat, DATA_PARAMETERS);
            vsprintf(buf + n, format, argptr);
            buf[sz] = 0;
            LOGW("%s", buf);
            SAFE_DELETE_ARRAY(buf);
        }else
        {
            int n = sprintf(atgStaticLogBuf, dateFormat, DATA_PARAMETERS);
            vsprintf(atgStaticLogBuf + n, format, argptr);
            atgStaticLogBuf[sz] = 0;
            LOGW("%s", atgStaticLogBuf);
        }
    }
#endif
    va_end(argptr);

#undef DATA_PARAMETERS
}

