#include "vulkan.h"
#include "detection/gpu/gpu.h"

FFVulkanResult* ffDetectVulkan()
{
    static bool initialized = false;
    static FFVulkanResult result;
    if(!initialized)
    {
        initialized = true;
        ffStrbufInit(&result.driver);
        ffStrbufInit(&result.apiVersion);
        ffStrbufInit(&result.conformanceVersion);
        ffStrbufInit(&result.instanceVersion);
        ffListInit(&result.gpus, sizeof(FFGPUResult));
        result.error = "Not supported on this platform";
    }
    return &result;
}
