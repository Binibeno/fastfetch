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

    time_t now = time(NULL);
    if (now == (time_t)-1)
        return "time() failed";

    result->bootTime = (uint64_t)boottime.tv_sec;
    result->uptime = (uint64_t)((now > boottime.tv_sec) ? (now - boottime.tv_sec) : 0);
    return NULL;
}
