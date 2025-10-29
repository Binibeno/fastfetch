#include "opengl.h"

const char* ffDetectOpenGL(FFOpenGLOptions* options, FFOpenGLResult* result)
{
    FF_UNUSED(options)
    if(result)
    {
        ffStrbufClear(&result->version);
        ffStrbufClear(&result->renderer);
        ffStrbufClear(&result->vendor);
        ffStrbufClear(&result->slv);
        ffStrbufClear(&result->library);
    }
    return "Not supported on this platform";
}
