/*  libps.cpp
 *
 *  Copyright (C) 2012 Willem Kemp <http://www.thenocturnaltree.com>
 *  All rights reserved.
 *
 *  This file is part of PortScope.
 *
 *  PortScope is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PortScope is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PortScope. If not, see http://www.gnu.org/licenses/.
 *
 */
#include "libps.h"
#include <windows.h>
#include <stdio.h>
#include <assert.h>


#define IOCTL_OPEN_PORT         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_WRITE_DATA)


/*---------------------------------------------------------------------------*/
typedef struct
{
    HANDLE transmitDataHandle;
    HANDLE receiveDataHandle;

    OVERLAPPED transmitDataTransfer;
    OVERLAPPED receiveDataTransfer;

    HANDLE transmitDataEvent;
    HANDLE receiveDataEvent;
    
    unsigned char* transmitDataBuffer;
    unsigned char* receiveDataBuffer;

    unsigned long bufferSize;

    int flags;


} LIBPS_OBJ;



/*---------------------------------------------------------------------------*/
static LIBPS_RESULT LIBPS_IssueTransmitDataRead(LIBPS_OBJ* obj);
static LIBPS_RESULT LIBPS_IssueReceiveDataRead(LIBPS_OBJ* obj);



/*---------------------------------------------------------------------------*/
LIBPS_HANDLE LIBPS_Create(const wchar_t* port, size_t bufferSize)
{
    LIBPS_OBJ* obj = (LIBPS_OBJ*)malloc(sizeof(LIBPS_OBJ));
    wchar_t deviceName[128];
    BOOL result = FALSE;

    if (obj) {

        /* Clear the structure */
        ZeroMemory(obj, sizeof(LIBPS_OBJ));

        /* Open handles to the driver */
        obj->transmitDataHandle = CreateFile(
            L"\\\\.\\PortScope\\transmit",
            (GENERIC_READ | GENERIC_WRITE),
            0,
            0,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL
            );

        obj->receiveDataHandle = CreateFile(
            L"\\\\.\\PortScope\\receive",
            (GENERIC_READ | GENERIC_WRITE),
            0,
            0,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL
            );


        if ((obj->transmitDataHandle != INVALID_HANDLE_VALUE) &&
            (obj->receiveDataHandle != INVALID_HANDLE_VALUE)) {

            DWORD bytesTransferred = 0;

            // Format the device name
            swprintf(deviceName, 128, L"\\DosDevices\\%s", port);

            // Attempt to hook the port
            result = DeviceIoControl(
                obj->transmitDataHandle,
                IOCTL_OPEN_PORT,
                deviceName,
                50,
                0,
                0,
                &bytesTransferred,
                0);

            if (result) {

                /* Clear the flags */
                obj->flags = 0;

                /* Create the signaling events */
                obj->transmitDataEvent = CreateEvent(0, FALSE, FALSE, 0);
                obj->receiveDataEvent = CreateEvent(0, FALSE, FALSE, 0);

                /* Create the buffers */
                obj->bufferSize = bufferSize;
                obj->transmitDataBuffer = malloc(bufferSize);
                obj->receiveDataBuffer = malloc(bufferSize);

                /* Create the overlapped structures */
                ZeroMemory(&obj->transmitDataTransfer, sizeof(OVERLAPPED));
                ZeroMemory(&obj->receiveDataTransfer, sizeof(OVERLAPPED));

                /* Set the events */
                obj->transmitDataTransfer.hEvent = obj->transmitDataEvent;
                obj->receiveDataTransfer.hEvent = obj->receiveDataEvent;


                /* Final check before issuing reads */
                if ((obj->transmitDataEvent != NULL) &&
                    (obj->receiveDataEvent != NULL) &&
                    (obj->transmitDataBuffer != NULL) &&
                    (obj->receiveDataBuffer != NULL)) {


                    /* Issue the initial operations */
                    LIBPS_IssueTransmitDataRead(obj);
                    LIBPS_IssueReceiveDataRead(obj);

                } else {

                }

            /* Failure to attach to the port */
            } else {
                CloseHandle(obj->transmitDataHandle);
                CloseHandle(obj->receiveDataHandle);
                free(obj);
                obj = 0;
            }

        /* Failure to open the portscope driver */
        } else {
            free(obj);
            obj = 0;
        }
    }


    return obj;
}


/*---------------------------------------------------------------------------*/
void LIBPS_Close(LIBPS_HANDLE handle)
{
    LIBPS_OBJ* obj = (LIBPS_OBJ*)handle;
    if (obj) {
        CloseHandle(obj->transmitDataEvent);
        CloseHandle(obj->receiveDataEvent);
        CloseHandle(obj->transmitDataHandle);
        CloseHandle(obj->receiveDataHandle);

        free(obj->transmitDataBuffer);
        free(obj->receiveDataBuffer);

        free(obj);
    }
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_WaitForData(LIBPS_HANDLE handle, int* flags)
{
    LIBPS_OBJ* obj = (LIBPS_OBJ*)handle;
    LIBPS_RESULT result = LIBPS_ERROR;
    HANDLE handles[2];
    DWORD waitResult;


    /* Only wait if there is no available data */
    if (obj->flags == 0) {

        /* Initialize the event array */
        handles[0] = obj->transmitDataEvent;
        handles[1] = obj->receiveDataEvent;

        /* Wait on the events */
        waitResult = WaitForMultipleObjects(
            2,
            handles,
            FALSE,
            INFINITE);

        switch (waitResult) {

            /* Transmit Event */
            case WAIT_OBJECT_0: {
                DWORD bytesTransferred = 0;
                if (GetOverlappedResult(
                    obj->transmitDataHandle,
                    &obj->transmitDataTransfer,
                    &bytesTransferred,
                    FALSE)) {

                    if (bytesTransferred > 0) {
                        obj->flags |= LIBPS_TRANSMIT_DATA_AVAILABLE;
                        result = LIBPS_OK;

                    } else {
                        result = LIBPS_IssueTransmitDataRead(obj);
                    }
                }

            } break;

            /* Receive Event */
            case (WAIT_OBJECT_0 + 1): {
                DWORD bytesTransferred = 0;
                if (GetOverlappedResult(
                    obj->receiveDataHandle,
                    &obj->receiveDataTransfer,
                    &bytesTransferred,
                    FALSE)) {

                    if (bytesTransferred > 0) {
                        obj->flags |= LIBPS_RECEIVE_DATA_AVAILABLE;
                        result = LIBPS_OK;
                    } else {
                        result = LIBPS_IssueReceiveDataRead(obj);
                    }
                }

            } break;

            case WAIT_TIMEOUT: {

            } break;

            case WAIT_FAILED: {

            } break;
        }
    }


    *flags = obj->flags;

    return result;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_ReadTransmitData(LIBPS_HANDLE handle, void* data, size_t* length)
{


    LIBPS_OBJ* obj = (LIBPS_OBJ*)handle;
    LIBPS_RESULT result = LIBPS_ERROR;

    if (obj->flags & LIBPS_TRANSMIT_DATA_AVAILABLE) {

        DWORD bytesTransferred = 0;
        if (GetOverlappedResult(
                obj->transmitDataHandle,
                &obj->transmitDataTransfer,
                &bytesTransferred,
                FALSE)) {       

            assert(bytesTransferred <= obj->bufferSize);

            /* Copy the data */
            memcpy(data, obj->transmitDataBuffer, bytesTransferred);
            *length = bytesTransferred;
            obj->flags = 0;

            /* Issue the next read */
            result = LIBPS_IssueTransmitDataRead(obj);
        }
    }


    return result;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_ReadReceiveData(LIBPS_HANDLE handle, void* data, size_t* length)
{

    LIBPS_OBJ* obj = (LIBPS_OBJ*)handle;
    LIBPS_RESULT result = LIBPS_ERROR;

    if (obj->flags & LIBPS_RECEIVE_DATA_AVAILABLE) {

        DWORD bytesTransferred = 0;
        if (GetOverlappedResult(
                obj->receiveDataHandle,
                &obj->receiveDataTransfer,
                &bytesTransferred,
                FALSE)) {       

            /* Copy the data */
            assert(bytesTransferred <= obj->bufferSize);
            memcpy(data, obj->receiveDataBuffer, bytesTransferred);
            *length = bytesTransferred;
            obj->flags = 0;

            /* Issue the next read */
            result = LIBPS_IssueReceiveDataRead(obj);
        }
    }


    return result;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_IssueTransmitDataRead(LIBPS_OBJ* obj)
{
    LIBPS_RESULT result = LIBPS_OK;

    ReadFile(obj->transmitDataHandle, obj->transmitDataBuffer, obj->bufferSize, 0, &obj->transmitDataTransfer);

    return result;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_IssueReceiveDataRead(LIBPS_OBJ* obj)
{
    LIBPS_RESULT result = LIBPS_OK;
    ReadFile(obj->receiveDataHandle, obj->receiveDataBuffer, obj->bufferSize, 0, &obj->receiveDataTransfer);
    return result;
}
