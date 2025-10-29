#include "uptime.h"

#include <sys/sysctl.h>
#include <sys/time.h>
#include <time.h>

const char* ffDetectUptime(FFUptimeResult* result)
{
    if (!result)
        return "Uptime result is null";

    struct timeval boottime = {};
    size_t size = sizeof(boottime);
    if (sysctl((int[]){CTL_KERN, KERN_BOOTTIME}, 2, &boottime, &size, NULL, 0) != 0 || boottime.tv_sec == 0)
        return "sysctl kern.boottime failed";

    struct timeval now = {};
    if (gettimeofday(&now, NULL) != 0)
        return "gettimeofday() failed";

    result->bootTime = (uint64_t) boottime.tv_sec;

    uint64_t nowMs = (uint64_t) now.tv_sec * 1000 + (uint64_t) now.tv_usec / 1000;
    uint64_t bootMs = (uint64_t) boottime.tv_sec * 1000 + (uint64_t) boottime.tv_usec / 1000;
    result->uptime = nowMs > bootMs ? nowMs - bootMs : 0;
    return NULL;
}
