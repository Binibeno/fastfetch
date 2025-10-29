#include "uptime.h"

const char* ffDetectUptime(FFUptimeResult* result)
{
    if(result)
        *result = (FFUptimeResult){0};
    return "Not supported on this platform";
}
