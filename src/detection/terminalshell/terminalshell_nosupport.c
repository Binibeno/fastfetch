#include "terminalshell.h"

#include "util/stringUtils.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

    const char* shellEnv = getenv("SHELL");
    if (ffStrSet(shellEnv))
    {
        ffStrbufSetS(&shellResult.exePath, shellEnv);

        const char* base = strrchr(shellEnv, '/');
        base = base ? base + 1 : shellEnv;
        ffStrbufSetS(&shellResult.exe, base);
        shellResult.exeName = shellResult.exe.chars;
        ffStrbufSet(&shellResult.processName, &shellResult.exe);
        ffStrbufSet(&shellResult.prettyName, &shellResult.exe);
    }

    const char* termEnv = getenv("TERM_PROGRAM");
    if (!ffStrSet(termEnv)) termEnv = getenv("TERM");

    ffStrbufInit(&terminalResult.processName);
    ffStrbufInit(&terminalResult.exe);
    ffStrbufInit(&terminalResult.prettyName);
    terminalResult.exeName = terminalResult.exe.chars;
    ffStrbufInit(&terminalResult.exePath);
    ffStrbufInit(&terminalResult.version);
    ffStrbufInit(&terminalResult.tty);
    terminalResult.pid = 0;
    terminalResult.ppid = 0;

    if (ffStrSet(termEnv))
    {
        ffStrbufSetS(&terminalResult.processName, termEnv);
        ffStrbufSet(&terminalResult.prettyName, &terminalResult.processName);
        ffStrbufSet(&terminalResult.exe, &terminalResult.processName);
        terminalResult.exeName = terminalResult.exe.chars;
    }

    const char* ttyPath = ttyname(STDIN_FILENO);
    if (!ttyPath) ttyPath = getenv("SSH_TTY");
    if (ffStrSet(ttyPath))
        ffStrbufSetS(&terminalResult.tty, ttyPath);
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
