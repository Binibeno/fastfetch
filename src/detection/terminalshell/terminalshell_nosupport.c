#include "terminalshell.h"

static bool shellInitialized = false;
static FFShellResult shellResult;
static FFTerminalResult terminalResult;

static void initShell(void)
{
    if(shellInitialized)
        return;
    shellInitialized = true;

    ffStrbufInit(&shellResult.processName);
    ffStrbufInit(&shellResult.exe);
    shellResult.exeName = shellResult.exe.chars;
    ffStrbufInit(&shellResult.exePath);
    ffStrbufInit(&shellResult.prettyName);
    ffStrbufInit(&shellResult.version);
    shellResult.pid = 0;
    shellResult.ppid = 0;
    shellResult.tty = -1;

    ffStrbufInit(&terminalResult.processName);
    ffStrbufInit(&terminalResult.exe);
    ffStrbufInit(&terminalResult.prettyName);
    terminalResult.exeName = terminalResult.exe.chars;
    ffStrbufInit(&terminalResult.exePath);
    ffStrbufInit(&terminalResult.version);
    ffStrbufInit(&terminalResult.tty);
    terminalResult.pid = 0;
    terminalResult.ppid = 0;
}

const FFShellResult* ffDetectShell()
{
    initShell();
    return &shellResult;
}

const FFTerminalResult* ffDetectTerminal()
{
    initShell();
    return &terminalResult;
}
