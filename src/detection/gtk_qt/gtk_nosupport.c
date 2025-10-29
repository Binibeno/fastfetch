#include "gtk_qt.h"

static FFGTKResult gtkResult;
static FFQtResult qtResult;

static void initGTKResult(void)
{
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;
    ffStrbufInit(&gtkResult.theme);
    ffStrbufInit(&gtkResult.icons);
    ffStrbufInit(&gtkResult.font);
    ffStrbufInit(&gtkResult.cursor);
    ffStrbufInit(&gtkResult.cursorSize);
    ffStrbufInit(&gtkResult.wallpaper);
}

static void initQtResult(void)
{
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;
    ffStrbufInit(&qtResult.widgetStyle);
    ffStrbufInit(&qtResult.colorScheme);
    ffStrbufInit(&qtResult.icons);
    ffStrbufInit(&qtResult.font);
    ffStrbufInit(&qtResult.wallpaper);
}

const FFGTKResult* ffDetectGTK2(void)
{
    initGTKResult();
    return &gtkResult;
}

const FFGTKResult* ffDetectGTK3(void)
{
    initGTKResult();
    return &gtkResult;
}

const FFGTKResult* ffDetectGTK4(void)
{
    initGTKResult();
    return &gtkResult;
}

const FFQtResult* ffDetectQt(void)
{
    initQtResult();
    return &qtResult;
}
