#ifndef __LIBPS_H__
#define __LIBPS_H__

#include <stddef.h>


typedef int LIBPS_RESULT;
typedef void* LIBPS_HANDLE;

#define LIBPS_TRANSMIT_DATA_AVAILABLE   0x00000001
#define LIBPS_RECEIVE_DATA_AVAILABLE    0x00000002



LIBPS_HANDLE LIBPS_Initialize(size_t bufferSize);
LIBPS_RESULT LIBPS_WaitForData(LIBPS_HANDLE handle, int* flags);
LIBPS_RESULT LIBPS_ReadTransmitData(LIBPS_HANDLE handle, void* data, size_t* length);
LIBPS_RESULT LIBPS_ReadReceiveData(LIBPS_HANDLE handle, void* data, size_t* length);


#endif