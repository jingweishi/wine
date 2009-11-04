/*
 * Win32 advapi functions
 *
 * Copyright 1995 Sven Verdoolaege
 * Copyright 1998 Juergen Schmied
 * Copyright 2003 Mike Hearn
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "winerror.h"
#include "winternl.h"
#include "wmistr.h"
#include "evntrace.h"

#include "wine/unicode.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(advapi);
WINE_DECLARE_DEBUG_CHANNEL(eventlog);

static inline LPWSTR SERV_dup( LPCSTR str )
{
    UINT len;
    LPWSTR wstr;

    if( !str )
        return NULL;
    len = MultiByteToWideChar( CP_ACP, 0, str, -1, NULL, 0 );
    wstr = HeapAlloc( GetProcessHeap(), 0, len*sizeof (WCHAR) );
    MultiByteToWideChar( CP_ACP, 0, str, -1, wstr, len );
    return wstr;
}

/******************************************************************************
 * BackupEventLogA [ADVAPI32.@]
 *
 * Saves the event log to a backup file.
 *
 * PARAMS
 *  hEventLog        [I] Handle to event log to backup.
 *  lpBackupFileName [I] Name of the backup file.
 *
 * RETURNS
 *  Success: nonzero. File lpBackupFileName will contain the contents of
 *           hEvenLog.
 *  Failure: zero.
 */
BOOL WINAPI BackupEventLogA( HANDLE hEventLog, LPCSTR lpBackupFileName )
{
    LPWSTR backupW;
    BOOL ret;

    backupW = SERV_dup(lpBackupFileName);
    ret = BackupEventLogW(hEventLog, backupW);
    HeapFree(GetProcessHeap(), 0, backupW);

    return ret;
}

/******************************************************************************
 * BackupEventLogW [ADVAPI32.@]
 *
 * See BackupEventLogA.
 */
BOOL WINAPI BackupEventLogW( HANDLE hEventLog, LPCWSTR lpBackupFileName )
{
    FIXME("(%p,%s) stub\n", hEventLog, debugstr_w(lpBackupFileName));

    if (!lpBackupFileName)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if (!hEventLog)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (GetFileAttributesW(lpBackupFileName) != INVALID_FILE_ATTRIBUTES)
    {
        SetLastError(ERROR_ALREADY_EXISTS);
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************
 * ClearEventLogA [ADVAPI32.@]
 *
 * Clears the event log and optionally saves the log to a backup file.
 *
 * PARAMS
 *  hEvenLog         [I] Handle to event log to clear.
 *  lpBackupFileName [I] Name of the backup file.
 *
 * RETURNS
 *  Success: nonzero. if lpBackupFileName != NULL, lpBackupFileName will 
 *           contain the contents of hEvenLog and the log will be cleared.
 *  Failure: zero. Fails if the event log is empty or if lpBackupFileName
 *           exists.
 */
BOOL WINAPI ClearEventLogA( HANDLE hEventLog, LPCSTR lpBackupFileName )
{
    LPWSTR backupW;
    BOOL ret;

    backupW = SERV_dup(lpBackupFileName);
    ret = ClearEventLogW(hEventLog, backupW);
    HeapFree(GetProcessHeap(), 0, backupW);

    return ret;
}

/******************************************************************************
 * ClearEventLogW [ADVAPI32.@]
 *
 * See ClearEventLogA.
 */
BOOL WINAPI ClearEventLogW( HANDLE hEventLog, LPCWSTR lpBackupFileName )
{
    FIXME("(%p,%s) stub\n", hEventLog, debugstr_w(lpBackupFileName));

    if (!hEventLog)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************
 * CloseEventLog [ADVAPI32.@]
 *
 * Closes a read handle to the event log.
 *
 * PARAMS
 *  hEventLog [I/O] Handle of the event log to close.
 *
 * RETURNS
 *  Success: nonzero
 *  Failure: zero
 */
BOOL WINAPI CloseEventLog( HANDLE hEventLog )
{
    FIXME("(%p) stub\n", hEventLog);

    if (!hEventLog)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************
 * ControlTraceW [ADVAPI32.@]
 *
 * Control a givel event trace session
 *
 */
ULONG WINAPI ControlTraceW( TRACEHANDLE hSession, LPCWSTR SessionName, PEVENT_TRACE_PROPERTIES Properties, ULONG control )
{
    FIXME("(%s, %s, %p, %d) stub\n", wine_dbgstr_longlong(hSession), debugstr_w(SessionName), Properties, control);
    return ERROR_SUCCESS;
}

/******************************************************************************
 * ControlTraceA [ADVAPI32.@]
 *
 * See ControlTraceW.
 *
 */
ULONG WINAPI ControlTraceA( TRACEHANDLE hSession, LPCSTR SessionName, PEVENT_TRACE_PROPERTIES Properties, ULONG control )
{
    FIXME("(%s, %s, %p, %d) stub\n", wine_dbgstr_longlong(hSession), debugstr_a(SessionName), Properties, control);
    return ERROR_SUCCESS;
}

/******************************************************************************
 * DeregisterEventSource [ADVAPI32.@]
 * 
 * Closes a write handle to an event log
 *
 * PARAMS
 *  hEventLog [I/O] Handle of the event log.
 *
 * RETURNS
 *  Success: nonzero
 *  Failure: zero
 */
BOOL WINAPI DeregisterEventSource( HANDLE hEventLog )
{
    FIXME("(%p) stub\n", hEventLog);
    return TRUE;
}

/******************************************************************************
 * EnableTrace [ADVAPI32.@]
 */
ULONG WINAPI EnableTrace( ULONG enable, ULONG flag, ULONG level, LPCGUID guid, TRACEHANDLE hSession )
{
    FIXME("(%d, 0x%x, %d, %s, %s): stub\n", enable, flag, level,
            debugstr_guid(guid), wine_dbgstr_longlong(hSession));

    return ERROR_SUCCESS;
}

/******************************************************************************
 * GetEventLogInformation [ADVAPI32.@]
 *
 * Retrieve some information about an event log.
 *
 * PARAMS
 *  hEventLog      [I]   Handle to an open event log.
 *  dwInfoLevel    [I]   Level of information (only EVENTLOG_FULL_INFO)
 *  lpBuffer       [I/O] The buffer for the returned information
 *  cbBufSize      [I]   The size of the buffer
 *  pcbBytesNeeded [O]   The needed bytes to hold the information
 *
 * RETURNS
 *  Success: TRUE. lpBuffer will hold the information and pcbBytesNeeded shows
 *           the needed buffer size.
 *  Failure: FALSE.
 */
BOOL WINAPI GetEventLogInformation( HANDLE hEventLog, DWORD dwInfoLevel, LPVOID lpBuffer, DWORD cbBufSize, LPDWORD pcbBytesNeeded)
{
    EVENTLOG_FULL_INFORMATION *efi;

    FIXME("(%p, %d, %p, %d, %p) stub\n", hEventLog, dwInfoLevel, lpBuffer, cbBufSize, pcbBytesNeeded);

    if (dwInfoLevel != EVENTLOG_FULL_INFO)
    {
        SetLastError(ERROR_INVALID_LEVEL);
        return FALSE;
    }

    if (!hEventLog)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (!lpBuffer || !pcbBytesNeeded)
    {
        /* FIXME: This will be handled properly when eventlog is moved
         * to a higher level
         */
        SetLastError(RPC_X_NULL_REF_POINTER);
        return FALSE;
    }

    *pcbBytesNeeded = sizeof(EVENTLOG_FULL_INFORMATION);
    if (cbBufSize < sizeof(EVENTLOG_FULL_INFORMATION))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    /* Pretend the log is not full */
    efi = (EVENTLOG_FULL_INFORMATION *)lpBuffer;
    efi->dwFull = 0;

    return TRUE;
}

/******************************************************************************
 * GetNumberOfEventLogRecords [ADVAPI32.@]
 *
 * Retrieves the number of records in an event log.
 *
 * PARAMS
 *  hEventLog       [I] Handle to an open event log.
 *  NumberOfRecords [O] Number of records in the log.
 *
 * RETURNS
 *  Success: nonzero. NumberOfRecords will contain the number of records in
 *           the log.
 *  Failure: zero
 */
BOOL WINAPI GetNumberOfEventLogRecords( HANDLE hEventLog, PDWORD NumberOfRecords )
{
    FIXME("(%p,%p) stub\n", hEventLog, NumberOfRecords);

    if (!NumberOfRecords)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if (!hEventLog)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    *NumberOfRecords = 0;

    return TRUE;
}

/******************************************************************************
 * GetOldestEventLogRecord [ADVAPI32.@]
 *
 * Retrieves the absolute record number of the oldest record in an even log.
 *
 * PARAMS
 *  hEventLog    [I] Handle to an open event log.
 *  OldestRecord [O] Absolute record number of the oldest record.
 *
 * RETURNS
 *  Success: nonzero. OldestRecord contains the record number of the oldest
 *           record in the log.
 *  Failure: zero 
 */
BOOL WINAPI GetOldestEventLogRecord( HANDLE hEventLog, PDWORD OldestRecord )
{
    FIXME("(%p,%p) stub\n", hEventLog, OldestRecord);

    if (!OldestRecord)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if (!hEventLog)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    *OldestRecord = 0;

    return TRUE;
}

/******************************************************************************
 * NotifyChangeEventLog [ADVAPI32.@]
 *
 * Enables an application to receive notification when an event is written
 * to an event log.
 *
 * PARAMS
 *  hEventLog [I] Handle to an event log.
 *  hEvent    [I] Handle to a manual-reset event object.
 *
 * RETURNS
 *  Success: nonzero
 *  Failure: zero
 */
BOOL WINAPI NotifyChangeEventLog( HANDLE hEventLog, HANDLE hEvent )
{
	FIXME("(%p,%p) stub\n", hEventLog, hEvent);
	return TRUE;
}

/******************************************************************************
 * OpenBackupEventLogA [ADVAPI32.@]
 *
 * Opens a handle to a backup event log.
 *
 * PARAMS
 *  lpUNCServerName [I] Universal Naming Convention name of the server on which
 *                      this will be performed.
 *  lpFileName      [I] Specifies the name of the backup file.
 *
 * RETURNS
 *  Success: Handle to the backup event log.
 *  Failure: NULL
 */
HANDLE WINAPI OpenBackupEventLogA( LPCSTR lpUNCServerName, LPCSTR lpFileName )
{
    LPWSTR uncnameW, filenameW;
    HANDLE handle;

    uncnameW = SERV_dup(lpUNCServerName);
    filenameW = SERV_dup(lpFileName);
    handle = OpenBackupEventLogW(uncnameW, filenameW);
    HeapFree(GetProcessHeap(), 0, uncnameW);
    HeapFree(GetProcessHeap(), 0, filenameW);

    return handle;
}

/******************************************************************************
 * OpenBackupEventLogW [ADVAPI32.@]
 *
 * See OpenBackupEventLogA.
 */
HANDLE WINAPI OpenBackupEventLogW( LPCWSTR lpUNCServerName, LPCWSTR lpFileName )
{
    FIXME("(%s,%s) stub\n", debugstr_w(lpUNCServerName), debugstr_w(lpFileName));

    if (!lpFileName)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (lpUNCServerName && lpUNCServerName[0])
    {
        FIXME("Remote server not supported\n");
        SetLastError(RPC_S_SERVER_UNAVAILABLE);
        return NULL;
    }

    if (GetFileAttributesW(lpFileName) == INVALID_FILE_ATTRIBUTES)
    {
        SetLastError(ERROR_FILE_NOT_FOUND);
        return NULL;
    }

    return (HANDLE)0xcafe4242;
}

/******************************************************************************
 * OpenEventLogA [ADVAPI32.@]
 *
 * Opens a handle to the specified event log.
 *
 * PARAMS
 *  lpUNCServerName [I] UNC name of the server on which the event log is
 *                      opened.
 *  lpSourceName    [I] Name of the log.
 *
 * RETURNS
 *  Success: Handle to an event log.
 *  Failure: NULL
 */
HANDLE WINAPI OpenEventLogA( LPCSTR uncname, LPCSTR source )
{
    LPWSTR uncnameW, sourceW;
    HANDLE handle;

    uncnameW = SERV_dup(uncname);
    sourceW = SERV_dup(source);
    handle = OpenEventLogW(uncnameW, sourceW);
    HeapFree(GetProcessHeap(), 0, uncnameW);
    HeapFree(GetProcessHeap(), 0, sourceW);

    return handle;
}

/******************************************************************************
 * OpenEventLogW [ADVAPI32.@]
 *
 * See OpenEventLogA.
 */
HANDLE WINAPI OpenEventLogW( LPCWSTR uncname, LPCWSTR source )
{
    FIXME("(%s,%s) stub\n", debugstr_w(uncname), debugstr_w(source));

    if (!source)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (uncname && uncname[0])
    {
        FIXME("Remote server not supported\n");
        SetLastError(RPC_S_SERVER_UNAVAILABLE);
        return NULL;
    }

    return (HANDLE)0xcafe4242;
}

/******************************************************************************
 * QueryAllTracesW [ADVAPI32.@]
 *
 * Query informations for started event trace sessions
 *
 */
ULONG WINAPI QueryAllTracesW( PEVENT_TRACE_PROPERTIES * parray, ULONG arraycount, PULONG psessioncount )
{
    FIXME("(%p, %d, %p) stub\n", parray, arraycount, psessioncount);

    if (psessioncount) *psessioncount = 0;
    return ERROR_SUCCESS;
}

/******************************************************************************
 * QueryAllTracesA [ADVAPI32.@]
 *
 * See QueryAllTracesW.
 */
ULONG WINAPI QueryAllTracesA( PEVENT_TRACE_PROPERTIES * parray, ULONG arraycount, PULONG psessioncount )
{
    FIXME("(%p, %d, %p) stub\n", parray, arraycount, psessioncount);

    if (psessioncount) *psessioncount = 0;
    return ERROR_SUCCESS;
}

/******************************************************************************
 * ReadEventLogA [ADVAPI32.@]
 *
 * Reads a whole number of entries from an event log.
 *
 * PARAMS
 *  hEventLog                [I] Handle of the event log to read.
 *  dwReadFlags              [I] see MSDN doc.
 *  dwRecordOffset           [I] Log-entry record number to start at.
 *  lpBuffer                 [O] Buffer for the data read.
 *  nNumberOfBytesToRead     [I] Size of lpBuffer.
 *  pnBytesRead              [O] Receives number of bytes read.
 *  pnMinNumberOfBytesNeeded [O] Receives number of bytes required for the
 *                               next log entry.
 *
 * RETURNS
 *  Success: nonzero
 *  Failure: zero
 */
BOOL WINAPI ReadEventLogA( HANDLE hEventLog, DWORD dwReadFlags, DWORD dwRecordOffset,
    LPVOID lpBuffer, DWORD nNumberOfBytesToRead, DWORD *pnBytesRead, DWORD *pnMinNumberOfBytesNeeded )
{
    FIXME("(%p,0x%08x,0x%08x,%p,0x%08x,%p,%p) stub\n", hEventLog, dwReadFlags,
          dwRecordOffset, lpBuffer, nNumberOfBytesToRead, pnBytesRead, pnMinNumberOfBytesNeeded);
    return FALSE;
}

/******************************************************************************
 * ReadEventLogW [ADVAPI32.@]
 *
 * See ReadEventLogA.
 */
BOOL WINAPI ReadEventLogW( HANDLE hEventLog, DWORD dwReadFlags, DWORD dwRecordOffset,
    LPVOID lpBuffer, DWORD nNumberOfBytesToRead, DWORD *pnBytesRead, DWORD *pnMinNumberOfBytesNeeded )
{
    FIXME("(%p,0x%08x,0x%08x,%p,0x%08x,%p,%p) stub\n", hEventLog, dwReadFlags,
          dwRecordOffset, lpBuffer, nNumberOfBytesToRead, pnBytesRead, pnMinNumberOfBytesNeeded);
    return FALSE;
}

/******************************************************************************
 * RegisterEventSourceA [ADVAPI32.@]
 *
 * Returns a registered handle to an event log.
 *
 * PARAMS
 *  lpUNCServerName [I] UNC name of the source server.
 *  lpSourceName    [I] Specifies the name of the event source to retrieve.
 *
 * RETURNS
 *  Success: Handle to the event log.
 *  Failure: NULL. Returns ERROR_INVALID_HANDLE if lpSourceName specifies the
 *           Security event log.
 */
HANDLE WINAPI RegisterEventSourceA( LPCSTR lpUNCServerName, LPCSTR lpSourceName )
{
    UNICODE_STRING lpUNCServerNameW;
    UNICODE_STRING lpSourceNameW;
    HANDLE ret;

    FIXME("(%s,%s): stub\n", debugstr_a(lpUNCServerName), debugstr_a(lpSourceName));

    RtlCreateUnicodeStringFromAsciiz(&lpUNCServerNameW, lpUNCServerName);
    RtlCreateUnicodeStringFromAsciiz(&lpSourceNameW, lpSourceName);
    ret = RegisterEventSourceW(lpUNCServerNameW.Buffer,lpSourceNameW.Buffer);
    RtlFreeUnicodeString (&lpUNCServerNameW);
    RtlFreeUnicodeString (&lpSourceNameW);
    return ret;
}

/******************************************************************************
 * RegisterEventSourceW [ADVAPI32.@]
 *
 * See RegisterEventSourceA.
 */
HANDLE WINAPI RegisterEventSourceW( LPCWSTR lpUNCServerName, LPCWSTR lpSourceName )
{
    FIXME("(%s,%s): stub\n", debugstr_w(lpUNCServerName), debugstr_w(lpSourceName));
    return (HANDLE)0xcafe4242;
}

/******************************************************************************
 * ReportEventA [ADVAPI32.@]
 *
 * Writes an entry at the end of an event log.
 *
 * PARAMS
 *  hEventLog   [I] Handle of an event log.
 *  wType       [I] See MSDN doc.
 *  wCategory   [I] Event category.
 *  dwEventID   [I] Event identifier.
 *  lpUserSid   [I] Current user's security identifier.
 *  wNumStrings [I] Number of insert strings in lpStrings.
 *  dwDataSize  [I] Size of event-specific raw data to write.
 *  lpStrings   [I] Buffer containing an array of string to be merged.
 *  lpRawData   [I] Buffer containing the binary data.
 *
 * RETURNS
 *  Success: nonzero. Entry was written to the log.
 *  Failure: zero.
 *
 * NOTES
 *  The ReportEvent function adds the time, the entry's length, and the
 *  offsets before storing the entry in the log. If lpUserSid != NULL, the
 *  username is also logged.
 */
BOOL WINAPI ReportEventA ( HANDLE hEventLog, WORD wType, WORD wCategory, DWORD dwEventID,
    PSID lpUserSid, WORD wNumStrings, DWORD dwDataSize, LPCSTR *lpStrings, LPVOID lpRawData)
{
    LPWSTR *wideStrArray;
    UNICODE_STRING str;
    int i;
    BOOL ret;

    FIXME("(%p,0x%04x,0x%04x,0x%08x,%p,0x%04x,0x%08x,%p,%p): stub\n", hEventLog,
          wType, wCategory, dwEventID, lpUserSid, wNumStrings, dwDataSize, lpStrings, lpRawData);

    if (wNumStrings == 0) return TRUE;
    if (!lpStrings) return TRUE;

    wideStrArray = HeapAlloc(GetProcessHeap(), 0, sizeof(LPWSTR) * wNumStrings);
    for (i = 0; i < wNumStrings; i++)
    {
        RtlCreateUnicodeStringFromAsciiz(&str, lpStrings[i]);
        wideStrArray[i] = str.Buffer;
    }
    ret = ReportEventW(hEventLog, wType, wCategory, dwEventID, lpUserSid,
                       wNumStrings, dwDataSize, (LPCWSTR *)wideStrArray, lpRawData);
    for (i = 0; i < wNumStrings; i++)
    {
        HeapFree( GetProcessHeap(), 0, wideStrArray[i] );
    }
    HeapFree(GetProcessHeap(), 0, wideStrArray);
    return ret;
}

/******************************************************************************
 * ReportEventW [ADVAPI32.@]
 *
 * See ReportEventA.
 */
BOOL WINAPI ReportEventW( HANDLE hEventLog, WORD wType, WORD wCategory, DWORD dwEventID,
    PSID lpUserSid, WORD wNumStrings, DWORD dwDataSize, LPCWSTR *lpStrings, LPVOID lpRawData )
{
    int i;

    FIXME("(%p,0x%04x,0x%04x,0x%08x,%p,0x%04x,0x%08x,%p,%p): stub\n", hEventLog,
          wType, wCategory, dwEventID, lpUserSid, wNumStrings, dwDataSize, lpStrings, lpRawData);

    /* partial stub */

    if (wNumStrings == 0) return TRUE;
    if (!lpStrings) return TRUE;

    for (i = 0; i < wNumStrings; i++)
    {
        switch (wType)
        {
        case EVENTLOG_SUCCESS:
            TRACE_(eventlog)("%s\n", debugstr_w(lpStrings[i]));
            break;
        case EVENTLOG_ERROR_TYPE:
            ERR_(eventlog)("%s\n", debugstr_w(lpStrings[i]));
            break;
        case EVENTLOG_WARNING_TYPE:
            WARN_(eventlog)("%s\n", debugstr_w(lpStrings[i]));
            break;
        default:
            TRACE_(eventlog)("%s\n", debugstr_w(lpStrings[i]));
            break;
        }
    }
    return TRUE;
}

/******************************************************************************
 * RegisterTraceGuidsW [ADVAPI32.@]
 *
 * Register an event trace provider and the event trace classes that it uses
 * to generate events.
 *
 * PARAMS
 *  RequestAddress     [I]   ControlCallback function
 *  RequestContext     [I]   Optional provider-defined context
 *  ControlGuid        [I]   GUID of the registering provider
 *  GuidCount          [I]   Number of elements in the TraceGuidReg array
 *  TraceGuidReg       [I/O] Array of TRACE_GUID_REGISTRATION structures
 *  MofImagePath       [I]   not supported, set to NULL
 *  MofResourceNmae    [I]   not supported, set to NULL
 *  RegistrationHandle [O]   Provider's registration handle
 *
 * RETURNS
 *  Success: ERROR_SUCCESS
 *  Failure: System error code
 *
 * FIXME
 *  Stub.
 */
ULONG WINAPI RegisterTraceGuidsW( WMIDPREQUEST RequestAddress,
                PVOID RequestContext, LPCGUID ControlGuid, ULONG GuidCount,
                PTRACE_GUID_REGISTRATION TraceGuidReg, LPCWSTR MofImagePath,
                LPCWSTR MofResourceName, PTRACEHANDLE RegistrationHandle )
{
    FIXME("%p %p %p %u %p %s %s %p\n", RequestAddress, RequestContext,
          ControlGuid, GuidCount, TraceGuidReg, debugstr_w(MofImagePath),
          debugstr_w(MofResourceName), RegistrationHandle);
    return ERROR_CALL_NOT_IMPLEMENTED;
}

/******************************************************************************
 * RegisterTraceGuidsA [ADVAPI32.@]
 *
 * See RegisterTraceGuidsW.
 *
 * FIXME
 *  Stub.
 */
ULONG WINAPI RegisterTraceGuidsA( WMIDPREQUEST RequestAddress,
                PVOID RequestContext, LPCGUID ControlGuid, ULONG GuidCount,
                PTRACE_GUID_REGISTRATION TraceGuidReg, LPCSTR MofImagePath,
                LPCSTR MofResourceName, PTRACEHANDLE RegistrationHandle )
{
    FIXME("%p %p %p %u %p %s %s %p\n", RequestAddress, RequestContext,
          ControlGuid, GuidCount, TraceGuidReg, debugstr_a(MofImagePath),
          debugstr_a(MofResourceName), RegistrationHandle);
    return ERROR_CALL_NOT_IMPLEMENTED;
}

/******************************************************************************
 * StartTraceW [ADVAPI32.@]
 *
 * Register and start an event trace session
 *
 */
ULONG WINAPI StartTraceW( PTRACEHANDLE pSessionHandle, LPCWSTR SessionName, PEVENT_TRACE_PROPERTIES Properties )
{
    FIXME("(%p, %s, %p) stub\n", pSessionHandle, debugstr_w(SessionName), Properties);
    if (pSessionHandle) *pSessionHandle = 0xcafe4242;
    return ERROR_SUCCESS;
}

/******************************************************************************
 * StartTraceA [ADVAPI32.@]
 *
 * See StartTraceW.
 *
 */
ULONG WINAPI StartTraceA( PTRACEHANDLE pSessionHandle, LPCSTR SessionName, PEVENT_TRACE_PROPERTIES Properties )
{
    FIXME("(%p, %s, %p) stub\n", pSessionHandle, debugstr_a(SessionName), Properties);
    if (pSessionHandle) *pSessionHandle = 0xcafe4242;
    return ERROR_SUCCESS;
}

/******************************************************************************
 * TraceEvent [ADVAPI32.@]
 */
ULONG WINAPI TraceEvent( TRACEHANDLE SessionHandle, PEVENT_TRACE_HEADER EventTrace )
{
    FIXME("%s %p\n", wine_dbgstr_longlong(SessionHandle), EventTrace);
    return ERROR_CALL_NOT_IMPLEMENTED;
}

/******************************************************************************
 * UnregisterTraceGuids [ADVAPI32.@]
 *
 * See RegisterTraceGuids
 *
 * FIXME
 *  Stub.
 */
ULONG WINAPI UnregisterTraceGuids( TRACEHANDLE RegistrationHandle )
{
    FIXME("%s: stub\n", wine_dbgstr_longlong(RegistrationHandle));
    return ERROR_CALL_NOT_IMPLEMENTED;
}
