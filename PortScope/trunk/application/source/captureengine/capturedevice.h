/******************************************************************************
    capturedevice.h
 ******************************************************************************/
#ifndef __CAPTUREDEVICE_H__
#define __CAPTUREDEVICE_H__

#include <windows.h>
#include <QString>

class CaptureDevice
{
public:
    CaptureDevice(const QString& deviceName);
    ~CaptureDevice();
    size_t captureData(void* buffer, size_t length);

private:
    HANDLE _handle;
};


#endif
