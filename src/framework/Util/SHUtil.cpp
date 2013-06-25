#include <SHUtil.h>
#include <SHNetworkDef.h>

uint32 NAMESPACE_SH_NAME::getTime()
{
    #if PLATFORM == PLATFORM_WINDOWS
        SYSTEMTIME now;
        GetSystemTime(&now);
        uint32 milisec = now.wHour * 3600 * 1000 + now.wMinute * 60 * 1000 + now.wSecond * 1000 + now.wMilliseconds;
        return (milisec);
    #else
        struct timeval now;
        gettimeofday(&now, NULL);
        uint32 milisec = now.tv_sec * 1000 + now.tv_usec / 1000.0;
        return(milisec);
    #endif
}
