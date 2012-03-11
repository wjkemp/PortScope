#include "libps.h"
#include <windows.h>


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

} LIBPS;


/*---------------------------------------------------------------------------*/
static LIBPS_RESULT LIBPS_IssueTransmitDataRead(LIBPS* obj);
static LIBPS_RESULT LIBPS_IssueReceiveDataRead(LIBPS* obj);



/*---------------------------------------------------------------------------*/
LIBPS_HANDLE LIBPS_Initialize(size_t bufferSize)
{
    LIBPS* obj = (LIBPS*)malloc(sizeof(LIBPS));
    if (obj) {


        /* Open handles to the driver */
        obj->transmitDataHandle = CreateFile(
            L"\\\\.\\PortScope\transmit",
            (GENERIC_READ | GENERIC_WRITE),
            (FILE_SHARE_READ | FILE_SHARE_WRITE),
            0,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL
            );

        obj->receiveDataHandle = CreateFile(
            L"\\\\.\\PortScope\receive",
            (GENERIC_READ | GENERIC_WRITE),
            (FILE_SHARE_READ | FILE_SHARE_WRITE),
            0,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL
            );


        /* Create the signaling events */
        obj->transmitDataEvent = CreateEvent(
            0,
            FALSE,
            FALSE,
            0);

        obj->receiveDataEvent = CreateEvent(
            0,
            FALSE,
            FALSE,
            0);


        /* Create the overlapped structures */
        ZeroMemory(&obj->transmitDataTransfer, sizeof(OVERLAPPED));
        ZeroMemory(&obj->receiveDataTransfer, sizeof(OVERLAPPED));

        obj->transmitDataTransfer.hEvent = obj->transmitDataEvent;
        obj->receiveDataTransfer.hEvent = obj->receiveDataEvent;


        LIBPS_IssueTransmitDataRead(obj);
        LIBPS_IssueReceiveDataRead(obj);

    }

    return obj;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_WaitForData(LIBPS_HANDLE handle, int* flags)
{
    LIBPS* obj = (LIBPS*)handle;
    HANDLE handles[2];
    DWORD waitResult;


    handles[0] = obj->transmitDataEvent;
    handles[1] = obj->receiveDataEvent;


    waitResult = WaitForMultipleObjects(
        2,
        handles,
        FALSE,
        INFINITE);

    switch (waitResult) {

        /* Transmit Event */
        case WAIT_OBJECT_0: {
            obj->flags |= LIBPS_TRANSMIT_DATA_AVAILABLE;

        } break;

        /* Receive Event */
        case (WAIT_OBJECT_0 + 1): {
            obj->flags |= LIBPS_RECEIVE_DATA_AVAILABLE;

        } break;

        case WAIT_TIMEOUT: {

        } break;

        case WAIT_FAILED: {

        } break;
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_ReadTransmitData(LIBPS_HANDLE handle, void* data, size_t* length)
{
    LIBPS* obj = (LIBPS*)handle;

    if (obj->flags & LIBPS_TRANSMIT_DATA_AVAILABLE) {


        obj->flags &= ~LIBPS_TRANSMIT_DATA_AVAILABLE;
        LIBPS_IssueTransmitDataRead(obj);
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_ReadReceiveData(LIBPS_HANDLE handle, void* data, size_t* length)
{
    LIBPS* obj = (LIBPS*)handle;

    if (obj->flags & LIBPS_RECEIVE_DATA_AVAILABLE) {


        obj->flags &= ~LIBPS_RECEIVE_DATA_AVAILABLE;
        LIBPS_IssueTransmitDataRead(obj);
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_IssueTransmitDataRead(LIBPS* obj)
{
    ReadFile(obj->transmitDataHandle, obj->transmitDataBuffer, obj->bufferSize, 0, &obj->transmitDataTransfer);
    return 0;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_IssueReceiveDataRead(LIBPS* obj)
{
    ReadFile(obj->receiveDataHandle, obj->receiveDataBuffer, obj->bufferSize, 0, &obj->receiveDataTransfer);
    return 0;
}
