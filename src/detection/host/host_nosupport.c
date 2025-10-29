#include "host.h"
#include "util/stringUtils.h"

#include <sys/sysctl.h>
#include <string.h>

static bool sysctlCopyString(const char* name, FFstrbuf* target)
{
    char buffer[256];
    size_t size = sizeof(buffer);
    if (sysctlbyname(name, buffer, &size, NULL, 0) != 0 || size == 0)
        return false;
    buffer[size - 1] = '\0';
    ffStrbufSetS(target, buffer);
    return true;
}

const char* ffDetectHost(FFHostResult* host)
{
    if (!host)
        return "Host result is null";

    if (host->family.length == 0)
        ffStrbufSetS(&host->family, "Apple");
    if (host->vendor.length == 0)
        ffStrbufSetS(&host->vendor, "Apple Inc.");

    char machine[256];
    size_t machineSize = sizeof(machine);
    if (sysctlbyname("hw.machine", machine, &machineSize, NULL, 0) == 0 && machineSize > 0)
    {
        machine[machineSize - 1] = '\0';
        ffStrbufSetS(&host->name, machine);

        if (ffStrbufStartsWithIgnCaseS(&host->name, "iPhone"))
            ffStrbufSetS(&host->family, "Apple iPhone");
        else if (ffStrbufStartsWithIgnCaseS(&host->name, "iPad"))
            ffStrbufSetS(&host->family, "Apple iPad");
        else if (ffStrbufStartsWithIgnCaseS(&host->name, "iPod"))
            ffStrbufSetS(&host->family, "Apple iPod");
    }

    sysctlCopyString("hw.model", &host->version);

    return NULL;
}
