#include "libps.h"
#include <windows.h>


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

        {            
            DWORD bytesTransferred = 0;
            DeviceIoControl(
                obj->transmitDataHandle,
                IOCTL_OPEN_PORT,
                L"\\DosDevices\\COM15",
                50,
                0,
                0,
                &bytesTransferred,
                0);
        }


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

        /* Create the buffers */
        obj->bufferSize = bufferSize;
        obj->transmitDataBuffer = malloc(bufferSize);
        obj->receiveDataBuffer = malloc(bufferSize);

        /* Create the overlapped structures */
        ZeroMemory(&obj->transmitDataTransfer, sizeof(OVERLAPPED));
        ZeroMemory(&obj->receiveDataTransfer, sizeof(OVERLAPPED));

        obj->transmitDataTransfer.hEvent = obj->transmitDataEvent;
        obj->receiveDataTransfer.hEvent = obj->receiveDataEvent;

        obj->flags = 0;
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


    if (obj->flags) {

    } else {

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
                DWORD bytesTransferred = 0;
                GetOverlappedResult(
                    obj->transmitDataHandle,
                    &obj->transmitDataTransfer,
                    &bytesTransferred,
                    FALSE);

                if (bytesTransferred > 0) {
                    obj->flags = LIBPS_TRANSMIT_DATA_AVAILABLE;
                } else {
                    LIBPS_IssueTransmitDataRead(obj);
                }

            } break;

            /* Receive Event */
            case (WAIT_OBJECT_0 + 1): {
                DWORD bytesTransferred = 0;
                GetOverlappedResult(
                    obj->receiveDataHandle,
                    &obj->receiveDataTransfer,
                    &bytesTransferred,
                    FALSE);

                if (bytesTransferred > 0) {
                    obj->flags = LIBPS_RECEIVE_DATA_AVAILABLE;
                } else {
                    LIBPS_IssueReceiveDataRead(obj);
                }

            } break;

            case WAIT_TIMEOUT: {

            } break;

            case WAIT_FAILED: {

            } break;
        }
    }

    *flags = obj->flags;

    return 0;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_ReadTransmitData(LIBPS_HANDLE handle, void* data, size_t* length)
{
    LIBPS* obj = (LIBPS*)handle;

    if (obj->flags == LIBPS_TRANSMIT_DATA_AVAILABLE) {

        DWORD bytesTransferred = 0;
        GetOverlappedResult(
            obj->transmitDataHandle,
            &obj->transmitDataTransfer,
            &bytesTransferred,
            FALSE);
        

        memcpy(data, obj->transmitDataBuffer, bytesTransferred);
        *length = bytesTransferred;

        obj->flags = 0;
        LIBPS_IssueTransmitDataRead(obj);
    }

    return 0;
}


/*---------------------------------------------------------------------------*/
LIBPS_RESULT LIBPS_ReadReceiveData(LIBPS_HANDLE handle, void* data, size_t* length)
{
    LIBPS* obj = (LIBPS*)handle;

    if (obj->flags == LIBPS_RECEIVE_DATA_AVAILABLE) {

        DWORD bytesTransferred = 0;
        GetOverlappedResult(
            obj->receiveDataHandle,
            &obj->receiveDataTransfer,
            &bytesTransferred,
            FALSE);
        

        memcpy(data, obj->receiveDataBuffer, bytesTransferred);
        *length = bytesTransferred;

        obj->flags = 0;
        LIBPS_IssueReceiveDataRead(obj);
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
