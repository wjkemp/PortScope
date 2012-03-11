/*******************************************************************************
    psentry.c
********************************************************************************/
#include <wdm.h>
#include "portscope.h"

 

/* 
 * These compiler directives tell the Operating System how to load the
 * driver into memory. The "INIT" section is discardable as you only
 * need the driver entry upon initialization, then it can be discarded.
 *
 */


/*-- Function Prototypes -----------------------------------------------------*/

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, PortScope_Unload)


/*----------------------------------------------------------------------------*/
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG index = 0;
    PDEVICE_OBJECT pDeviceObject = NULL;
    UNICODE_STRING usDriverName, usDosDeviceName;
    PCONTROL_DEVICE_EXTENSION deviceExtension;

    UNREFERENCED_PARAMETER(pRegistryPath);

    
    DBG0(("PortScope: DriverEntry\n"));

    RtlInitUnicodeString(&usDriverName, L"\\Device\\PortScope");
    RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\PortScope"); 

    /* Create the device object */
    status = IoCreateDevice(
        pDriverObject,
        sizeof(CONTROL_DEVICE_EXTENSION),
        &usDriverName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &pDeviceObject);

    
    if (NT_SUCCESS(status)) {

        /* Initialize unused functions */
        for (index = 0; index < IRP_MJ_MAXIMUM_FUNCTION; index++) {
             pDriverObject->MajorFunction[index] = PortScope_DispatchUnknown;
        }
    
        /* Set Major Functions */
        pDriverObject->MajorFunction[IRP_MJ_CREATE]             = PortScope_DispatchCreate;
        pDriverObject->MajorFunction[IRP_MJ_CLOSE]              = PortScope_DispatchClose;
        pDriverObject->MajorFunction[IRP_MJ_READ]               = PortScope_DispatchRead;
        pDriverObject->MajorFunction[IRP_MJ_WRITE]              = PortScope_DispatchWrite;
        pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]     = PortScope_DispatchIoControl;
        pDriverObject->MajorFunction[IRP_MJ_PNP]			          = PortScope_DispatchPnp;
        pDriverObject->MajorFunction[IRP_MJ_POWER]              = PortScope_DispatchPower;
        pDriverObject->DriverUnload                             = PortScope_Unload; 

        /* Do Buffered IO */
        pDeviceObject->Flags |= DO_BUFFERED_IO;
        
        /* Signal Initialization done */
        pDeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);
            
        /* Create symbolic device link */
        IoCreateSymbolicLink(&usDosDeviceName, &usDriverName);

        /* Initialize the control device extension */
        deviceExtension = (PCONTROL_DEVICE_EXTENSION)pDeviceObject->DeviceExtension;
        deviceExtension->Common.Type = DEVICE_TYPE_CONTROL;

        /* Initialize the buffers */
        Buffer_Initialize(&deviceExtension->WriteBuffer, deviceExtension->WriteBufferData, sizeof(deviceExtension->WriteBufferData));
        KeInitializeSpinLock(&deviceExtension->WriteLock);

        Buffer_Initialize(&deviceExtension->ReadBuffer, deviceExtension->ReadBufferData, sizeof(deviceExtension->ReadBufferData));
        KeInitializeSpinLock(&deviceExtension->ReadLock);


        /* Initialize the read/write engines */
        RwEngine_Initialize(
            &deviceExtension->TransmitDataEngine,
            PortScope_RwNullFunction,
            PortScope_ReadTransmitData,
            deviceExtension);

        RwEngine_Initialize(
            &deviceExtension->ReceiveDataEngine,
            PortScope_RwNullFunction,
            PortScope_ReadReceiveData,
            deviceExtension);
    }


    return status;
}


/*----------------------------------------------------------------------------*/
VOID PortScope_Unload(PDRIVER_OBJECT DriverObject)
{        
    UNICODE_STRING usDosDeviceName;
    PDEVICE_OBJECT deviceObject;
    PCOMMON_DEVICE_DATA commonData;

    PAGED_CODE();
    
    DBG0(("PortScope: Unload\n"));

    deviceObject = DriverObject->DeviceObject;
    while (deviceObject) {
        commonData = (PCOMMON_DEVICE_DATA)deviceObject->DeviceExtension;
        DbgPrint("PortScope: Device object has type %d, next device is %08X\n", commonData->Type, deviceObject->NextDevice);

        if (commonData->Type == DEVICE_TYPE_FILTER) {
            PFILTER_DEVICE_EXTENSION filterDeviceExtension = (PFILTER_DEVICE_EXTENSION)deviceObject->DeviceExtension;
            
            DbgPrint("PortScope: Removing Filter Driver");
            IoDetachDevice(filterDeviceExtension->NextLowerDriver);
            IoDeleteDevice(deviceObject);
        }

        deviceObject = deviceObject->NextDevice;
    }
        
    /* Delete the symbolic link */
    RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\PortScope");
    IoDeleteSymbolicLink(&usDosDeviceName);

    /* Delete the device */
    IoDeleteDevice(DriverObject->DeviceObject);
}
