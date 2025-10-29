#include "os.h"
#include "util/stringUtils.h"

#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool plistCopyString(const char* content, const char* key, FFstrbuf* target)
{
    const char* keyTagPrefix = "<key>";
    const char* keyTagSuffix = "</key>";
    FF_STRBUF_AUTO_DESTROY searchKey = ffStrbufCreateA(64);
    ffStrbufAppendS(&searchKey, keyTagPrefix);
    ffStrbufAppendS(&searchKey, key);
    ffStrbufAppendS(&searchKey, keyTagSuffix);

    const char* keyPos = strstr(content, searchKey.chars);
    if (!keyPos)
        return false;

    const char* stringTagOpen = strstr(keyPos, "<string>");
    if (!stringTagOpen)
        return false;
    stringTagOpen += strlen("<string>");

    const char* stringTagClose = strstr(stringTagOpen, "</string>");
    if (!stringTagClose)
        return false;

    ffStrbufSetNS(target, (uint32_t)(stringTagClose - stringTagOpen), stringTagOpen);
    return true;
}

void ffDetectOSImpl(FFOSResult* os)
{
    ffStrbufSetS(&os->id, "ios");
    ffStrbufSetS(&os->idLike, "darwin");

    const char* plistPath = "/System/Library/CoreServices/SystemVersion.plist";
    FILE* fp = fopen(plistPath, "rb");
    if (fp)
    {
        if (fseek(fp, 0, SEEK_END) == 0)
        {
            long len = ftell(fp);
            if (len > 0 && fseek(fp, 0, SEEK_SET) == 0)
            {
                char* buffer = (char*)malloc((size_t)len + 1);
                if (buffer)
                {
                    size_t readLen = fread(buffer, 1, (size_t)len, fp);
                    buffer[readLen] = '\0';

                    plistCopyString(buffer, "ProductName", &os->name);
                    plistCopyString(buffer, "ProductVersion", &os->version);
                    plistCopyString(buffer, "ProductBuildVersion", &os->buildID);

                    free(buffer);
                }
            }
        }
        fclose(fp);
    }

    struct utsname uts;
    if (uname(&uts) == 0)
    {
        if (os->version.length == 0)
            ffStrbufSetS(&os->version, uts.release);
        if (os->name.length == 0)
            ffStrbufSetS(&os->name, uts.sysname);
    }

    if (os->prettyName.length == 0)
    {
        if (os->buildID.length > 0)
            ffStrbufSetF(&os->prettyName, "%s %s (%s)", os->name.chars, os->version.chars, os->buildID.chars);
        else
            ffStrbufSetF(&os->prettyName, "%s %s", os->name.chars, os->version.chars);
    }

    if (os->versionID.length == 0)
        ffStrbufSet(&os->versionID, &os->version);
}
