#include "terminalfont.h"
#include "detection/terminalshell/terminalshell.h"

bool ffDetectTerminalFontPlatform(const FFTerminalResult* terminal, FFTerminalFontResult* terminalFont)
{
    FF_UNUSED(terminal, terminalFont)
    return false;
}
