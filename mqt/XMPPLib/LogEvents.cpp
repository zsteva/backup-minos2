/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//
// COPYRIGHT         (c) M. J. Goodey G0GJV 2005 - 2008
//
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#include "XMPP_pch.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
#else
#include "unistd.h"
#endif

//---------------------------------------------------------------------------
void logMessage( const QString &level, const QString &mess )
{
    // queue a log event on the event queue

    trace( "Log : " + level + " " + mess );

}

// How to find if parent has disappeared - this needs to go in the children
// So we need a "starter" process for where we don't want the logger (e.g. voice keyer)


static unsigned long currentPID()
{
#ifdef Q_OS_WIN
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

static unsigned long parentPID()
{
#ifdef Q_OS_WIN

    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);

    unsigned long pid = currentPID();
    unsigned long ppid = 0;

    if( Process32First(h, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                ppid = pe.th32ParentProcessID;
                break;
            }
        } while( Process32Next(h, &pe));
    }

    CloseHandle(h);

    return ppid;

#else

    return getppid();

#endif
}

static bool isParentRunning()
{
#ifdef Q_OS_WIN

    static unsigned long _parentPID = parentPID();
    static void* _parentHandle = NULL;

    if (_parentHandle == NULL && _parentPID != 0)
        _parentHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, _parentPID);

    if (_parentHandle != NULL)
    {
        BOOL success;
        DWORD exitCode;

        success = GetExitCodeProcess(_parentHandle, &exitCode);

        return ( ! success) || exitCode == STILL_ACTIVE;
    }

    return true;

#else
    return getppid() != 1;
#endif
}

//---------------------------------------------------------------------------
void resetCloseEvent()
{
}
//---------------------------------------------------------------------------
void createCloseEvent()
{
}
//---------------------------------------------------------------------------
void signalCloseEvent()
{
}
//---------------------------------------------------------------------------
bool checkCloseEvent()
{
    return isParentRunning();
}
//---------------------------------------------------------------------------
