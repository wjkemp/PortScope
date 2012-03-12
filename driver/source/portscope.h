#ifndef __PORTSCOPE_H__
#define __PORTSCOPE_H__

#include <ntddk.h>
#include "psbuffer.h"
#include "rwengine.h"


#define POOL_TAG   'liFT'


#define DBG0(_x_) /* DbgPrint(_x_) */
#define DBG1(_x_) DbgPrint(_x_)
#define DBG2(_x_) DbgPrint(_x_)
#define DBG3(_x_) DbgPrint(_x_)



/*
#define DBG0(_x_)
#define DBG1(_x_)
#define DBG2(_x_)
#define DBG3(_x_)
*/


DRIVER_INITIALIZE DriverEntry;

__drv_dispatchType(IRP_MJ_CREATE)
DRIVER_DISPATCH PortScope_DispatchCreate;

__drv_dispatchType(IRP_MJ_CLOSE)
DRIVER_DISPATCH PortScope_DispatchClose;

__drv_dispatchType(IRP_MJ_READ)
DRIVER_DISPATCH PortScope_DispatchRead;

__drv_dispatchType(IRP_MJ_WRITE)
DRIVER_DISPATCH PortScope_DispatchWrite;

__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH PortScope_DispatchIoControl;

__drv_dispatchType(IRP_MJ_PNP)
DRIVER_DISPATCH PortScope_DispatchPnp;

__drv_dispatchType(IRP_MJ_POWER)
DRIVER_DISPATCH PortScope_DispatchPower;

__drv_dispatchType(IRP_MJ_CREATE)
__drv_dispatchType(IRP_MJ_CLOSE)
__drv_dispatchType(IRP_MJ_READ)
__drv_dispatchType(IRP_MJ_WRITE)
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH PortScope_DispatchUnknown;





NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath); 
VOID PortScope_Unload(PDRIVER_OBJECT  DriverObject);    



NTSTATUS PortScope_DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchRead(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchPower(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchUnknown(PDEVICE_OBJECT DeviceObject, PIRP Irp);


NTSTATUS PortScope_ControlCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlRead(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlPower(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlUnknown(PDEVICE_OBJECT DeviceObject, PIRP Irp);


NTSTATUS PortScope_FilterCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterRead(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterPower(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterUnknown(PDEVICE_OBJECT DeviceObject, PIRP Irp);


NTSTATUS FilterStartCompletionRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context);
NTSTATUS FilterDeviceUsageNotificationCompletionRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context);

NTSTATUS PortScope_InstallFilterDriver(PDEVICE_OBJECT ControlDevice, PUNICODE_STRING deviceName);


ULONG PortScope_ReadTransmitData(PVOID context, PVOID buffer, ULONG length);
ULONG PortScope_ReadReceiveData(PVOID context, PVOID buffer, ULONG length);
ULONG PortScope_RwNullFunction(PVOID context, PVOID buffer, ULONG length);




/*-----------------------------------------------------------------------------
    Device Extensions
 ------------------------------------------------------------------------------*/
typedef enum _DEVICE_TYPE {

    DEVICE_TYPE_INVALID = 0,
    DEVICE_TYPE_CONTROL,
    DEVICE_TYPE_FILTER,

} DEVICE_TYPE;




typedef enum _DEVICE_PNP_STATE {

    NotStarted = 0,         // Not started yet
    Started,                // Device has received the START_DEVICE IRP
    StopPending,            // Device has received the QUERY_STOP IRP
    Stopped,                // Device has received the STOP_DEVICE IRP
    RemovePending,          // Device has received the QUERY_REMOVE IRP
    SurpriseRemovePending,  // Device has received the SURPRISE_REMOVE IRP
    Deleted                 // Device has received the REMOVE_DEVICE IRP

} DEVICE_PNP_STATE;

#define INITIALIZE_PNP_STATE(_Data_)    \
        (_Data_)->DevicePnPState =  NotStarted;\
        (_Data_)->PreviousPnPState = NotStarted;

#define SET_NEW_PNP_STATE(_Data_, _state_) \
        (_Data_)->PreviousPnPState =  (_Data_)->DevicePnPState;\
        (_Data_)->DevicePnPState = (_state_);

#define RESTORE_PREVIOUS_PNP_STATE(_Data_)   \
        (_Data_)->DevicePnPState =   (_Data_)->PreviousPnPState;\
        
        


typedef struct
{
  ULONG Type;
  
} COMMON_DEVICE_DATA, *PCOMMON_DEVICE_DATA;



typedef struct
{
    COMMON_DEVICE_DATA Common;
    BUFFER WriteBuffer;
    unsigned char WriteBufferData[1024];

    BUFFER ReadBuffer;
    unsigned char ReadBufferData[1024];

    ULONG TransmitDataTag;
    ULONG ReceiveDataTag;

    RWENGINE TransmitDataEngine;
    RWENGINE ReceiveDataEngine;

    LIST_ENTRY FilterDeviceList;   

} CONTROL_DEVICE_EXTENSION, *PCONTROL_DEVICE_EXTENSION;



typedef struct
{
    COMMON_DEVICE_DATA Common;
    PDEVICE_OBJECT Self;
    PDEVICE_OBJECT NextLowerDriver;
    DEVICE_PNP_STATE DevicePnPState;
    DEVICE_PNP_STATE PreviousPnPState;
    IO_REMOVE_LOCK RemoveLock;    
    PCONTROL_DEVICE_EXTENSION ControlDevice;
    ULONG IrpsDispatched;
    ULONG IrpsCompleted;
    UNICODE_STRING DeviceName;
    LIST_ENTRY ListEntry;   

} FILTER_DEVICE_EXTENSION, *PFILTER_DEVICE_EXTENSION;



/*-----------------------------------------------------------------------------
    IO Control Codes
 ------------------------------------------------------------------------------*/
#define IOCTL_OPEN_PORT         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_WRITE_DATA)

#endif
