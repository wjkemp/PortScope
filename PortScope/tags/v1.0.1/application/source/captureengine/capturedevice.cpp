/******************************************************************************
    capturedevice.cpp
 ******************************************************************************/
#include "capturedevice.h"


#define IOCTL_OPEN_PORT         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_IN_DIRECT, FILE_WRITE_DATA)




//-----------------------------------------------------------------------------
CaptureDevice::CaptureDevice(const QString& deviceName)
{
    _handle = CreateFile(
        deviceName.toStdWString().c_str(),
        GENERIC_READ|GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        0,
        0);

    if (_handle == INVALID_HANDLE_VALUE) {
        throw std::exception();
    }

    DWORD bytesTransferred = 0;
    //DeviceIoControl(_handle, IOCTL_OPEN_PORT, 0, 0, 0, 0, &bytesTransferred, 0);

    
    /*
    COMMTIMEOUTS timeouts;
    DCB dcb;

    GetCommState(_handle, &dcb);
    BuildCommDCB(L"baud=115200 parity=N data=8 stop=1", &dcb);
    SetCommState(_handle, &dcb);

    GetCommTimeouts(_handle, &timeouts);
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadIntervalTimeout = 10;
    SetCommTimeouts(_handle, &timeouts);
    */
    
}


//-----------------------------------------------------------------------------
CaptureDevice::~CaptureDevice()
{
    CloseHandle(_handle);
}


//-----------------------------------------------------------------------------
size_t CaptureDevice::captureData(void* buffer, size_t length)
{
    DWORD bytesRead = 0;
    if (!ReadFile(_handle, buffer, length, &bytesRead, 0)) {
        throw std::exception();
    }

    return (size_t)bytesRead;
}
