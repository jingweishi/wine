/*
 * Copyright 2007 Misha Koshelev
 * Copyright 2009 Jacek Caban for CodeWeavers
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

#include "urlmon_main.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(urlmon);

/* Flags are needed for, among other things, return HRESULTs from the Read function
 * to conform to native. For example, Read returns:
 *
 * 1. E_PENDING if called before the request has completed,
 *        (flags = 0)
 * 2. S_FALSE after all data has been read and S_OK has been reported,
 *        (flags = FLAG_REQUEST_COMPLETE | FLAG_ALL_DATA_READ | FLAG_RESULT_REPORTED)
 * 3. INET_E_DATA_NOT_AVAILABLE if InternetQueryDataAvailable fails. The first time
 *    this occurs, INET_E_DATA_NOT_AVAILABLE will also be reported to the sink,
 *        (flags = FLAG_REQUEST_COMPLETE)
 *    but upon subsequent calls to Read no reporting will take place, yet
 *    InternetQueryDataAvailable will still be called, and, on failure,
 *    INET_E_DATA_NOT_AVAILABLE will still be returned.
 *        (flags = FLAG_REQUEST_COMPLETE | FLAG_RESULT_REPORTED)
 *
 * FLAG_FIRST_DATA_REPORTED and FLAG_LAST_DATA_REPORTED are needed for proper
 * ReportData reporting. For example, if OnResponse returns S_OK, Continue will
 * report BSCF_FIRSTDATANOTIFICATION, and when all data has been read Read will
 * report BSCF_INTERMEDIATEDATANOTIFICATION|BSCF_LASTDATANOTIFICATION. However,
 * if OnResponse does not return S_OK, Continue will not report data, and Read
 * will report BSCF_FIRSTDATANOTIFICATION|BSCF_LASTDATANOTIFICATION when all
 * data has been read.
 */
#define FLAG_REQUEST_COMPLETE         0x0001
#define FLAG_FIRST_CONTINUE_COMPLETE  0x0002
#define FLAG_FIRST_DATA_REPORTED      0x0004
#define FLAG_ALL_DATA_READ            0x0008
#define FLAG_LAST_DATA_REPORTED       0x0010
#define FLAG_RESULT_REPORTED          0x0020

static inline HRESULT report_result(Protocol *protocol, HRESULT hres)
{
    if (!(protocol->flags & FLAG_RESULT_REPORTED) && protocol->protocol_sink) {
        protocol->flags |= FLAG_RESULT_REPORTED;
        IInternetProtocolSink_ReportResult(protocol->protocol_sink, hres, 0, NULL);
    }

    return hres;
}

static void report_data(Protocol *protocol)
{
    DWORD bscf;

    if((protocol->flags & FLAG_LAST_DATA_REPORTED) || !protocol->protocol_sink)
        return;

    if(protocol->flags & FLAG_FIRST_DATA_REPORTED) {
        bscf = BSCF_INTERMEDIATEDATANOTIFICATION;
    }else {
        protocol->flags |= FLAG_FIRST_DATA_REPORTED;
        bscf = BSCF_FIRSTDATANOTIFICATION;
    }

    if(protocol->flags & FLAG_ALL_DATA_READ && !(protocol->flags & FLAG_LAST_DATA_REPORTED)) {
        protocol->flags |= FLAG_LAST_DATA_REPORTED;
        bscf |= BSCF_LASTDATANOTIFICATION;
    }

    IInternetProtocolSink_ReportData(protocol->protocol_sink, bscf,
            protocol->current_position+protocol->available_bytes,
            protocol->content_length);
}

static void all_data_read(Protocol *protocol)
{
    protocol->flags |= FLAG_ALL_DATA_READ;

    report_data(protocol);
    report_result(protocol, S_OK);
}

HRESULT protocol_read(Protocol *protocol, void *buf, ULONG size, ULONG *read_ret)
{
    ULONG read = 0;
    BOOL res;
    HRESULT hres = S_FALSE;

    if(!(protocol->flags & FLAG_REQUEST_COMPLETE)) {
        *read_ret = 0;
        return E_PENDING;
    }

    if(protocol->flags & FLAG_ALL_DATA_READ) {
        *read_ret = 0;
        return S_FALSE;
    }

    while(read < size) {
        if(protocol->available_bytes) {
            ULONG len;

            res = InternetReadFile(protocol->request, ((BYTE *)buf)+read,
                    protocol->available_bytes > size-read ? size-read : protocol->available_bytes, &len);
            if(!res) {
                WARN("InternetReadFile failed: %d\n", GetLastError());
                hres = INET_E_DOWNLOAD_FAILURE;
                report_result(protocol, hres);
                break;
            }

            if(!len) {
                all_data_read(protocol);
                break;
            }

            read += len;
            protocol->current_position += len;
            protocol->available_bytes -= len;
        }else {
            /* InternetQueryDataAvailable may immediately fork and perform its asynchronous
             * read, so clear the flag _before_ calling so it does not incorrectly get cleared
             * after the status callback is called */
            protocol->flags &= ~FLAG_REQUEST_COMPLETE;
            res = InternetQueryDataAvailable(protocol->request, &protocol->available_bytes, 0, 0);
            if(!res) {
                if (GetLastError() == ERROR_IO_PENDING) {
                    hres = E_PENDING;
                }else {
                    WARN("InternetQueryDataAvailable failed: %d\n", GetLastError());
                    hres = INET_E_DATA_NOT_AVAILABLE;
                    report_result(protocol, hres);
                }
                break;
            }

            if(!protocol->available_bytes) {
                all_data_read(protocol);
                break;
            }
        }
    }

    *read_ret = read;

    if (hres != E_PENDING)
        protocol->flags |= FLAG_REQUEST_COMPLETE;
    if(FAILED(hres))
        return hres;

    return read ? S_OK : S_FALSE;
}

HRESULT protocol_lock_request(Protocol *protocol)
{
    if (!InternetLockRequestFile(protocol->request, &protocol->lock))
        WARN("InternetLockRequest failed: %d\n", GetLastError());

    return S_OK;
}

HRESULT protocol_unlock_request(Protocol *protocol)
{
    if(!protocol->lock)
        return S_OK;

    if(!InternetUnlockRequestFile(protocol->lock))
        WARN("InternetUnlockRequest failed: %d\n", GetLastError());
    protocol->lock = 0;

    return S_OK;
}

void protocol_close_connection(Protocol *protocol)
{
    protocol->vtbl->close_connection(protocol);

    if(protocol->request)
        InternetCloseHandle(protocol->request);
    if(protocol->internet) {
        InternetCloseHandle(protocol->internet);
        protocol->internet = 0;
    }

    protocol->flags = 0;
}
