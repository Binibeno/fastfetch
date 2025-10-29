#include "os.h"

void ffDetectOSImpl(FFOSResult* os)
{
    ffStrbufSetS(&os->name, "iOS");
    if(os->prettyName.length == 0)
        ffStrbufSetS(&os->prettyName, "iOS");
    ffStrbufSetS(&os->id, "ios");
}
