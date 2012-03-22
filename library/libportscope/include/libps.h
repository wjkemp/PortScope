#ifndef __LIBPS_H__
#define __LIBPS_H__

#include <stddef.h>


#ifdef LIBPORTSCOPE_EXPORTS
#define LIBPS_API __declspec(dllexport)
#else
#define LIBPS_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef int LIBPS_RESULT;
typedef void* LIBPS_HANDLE;

#define LIBPS_TRANSMIT_DATA_AVAILABLE   0x00000001
#define LIBPS_RECEIVE_DATA_AVAILABLE    0x00000002

#define LIBPS_OK                        0
#define LIBPS_ERROR                     1


LIBPS_API LIBPS_HANDLE LIBPS_Create(const wchar_t* port, size_t bufferSize);
LIBPS_API void LIBPS_Close(LIBPS_HANDLE handle);
LIBPS_API LIBPS_RESULT LIBPS_WaitForData(LIBPS_HANDLE handle, int* flags);
LIBPS_API LIBPS_RESULT LIBPS_ReadTransmitData(LIBPS_HANDLE handle, void* data, size_t* length);
LIBPS_API LIBPS_RESULT LIBPS_ReadReceiveData(LIBPS_HANDLE handle, void* data, size_t* length);

#ifdef __cplusplus
}
#endif



#endif