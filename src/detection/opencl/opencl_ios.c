#include "opencl.h"
#include "detection/gpu/gpu.h"

FFOpenCLResult* ffDetectOpenCL()
{
    static bool initialized = false;
    static FFOpenCLResult result;
    if(!initialized)
    {
        initialized = true;
        ffStrbufInit(&result.version);
        ffStrbufInit(&result.name);
        ffStrbufInit(&result.vendor);
        ffListInit(&result.gpus, sizeof(FFGPUResult));
        result.error = "Not supported on this platform";
    }
    return &result;
}
