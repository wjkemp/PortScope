/*  psentry.c - Driver Entry Point
 *
 *  Copyright 2012 Willem Kemp.
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
#include "psdispatch.h"
#include "pscontrol.h"
#include "psfilter.h"



/*---------------------------------------------------------------------------
    Defines
 ----------------------------------------------------------------------------*/
DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD PortScope_Unload;

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, PortScope_Unload)


/*---------------------------------------------------------------------------
    Variables
 ----------------------------------------------------------------------------*/ 


/*---------------------------------------------------------------------------
    Functions
 ----------------------------------------------------------------------------*/
NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath); 
VOID PortScope_Unload(PDRIVER_OBJECT  DriverObject);    



/*----------------------------------------------------------------------------*/
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG index = 0;
    PDEVICE_OBJECT pDeviceObject = NULL;
    UNICODE_STRING usDriverName, usDosDeviceName;
    PCONTROL_DEVICE_EXTENSION deviceExtension;

    UNREFERENCED_PARAMETER(pRegistryPath);

    
    DBG1(("PortScope: DriverEntry\n"));

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
        PortScope_ControlInitializeDeviceExtension(deviceExtension);
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
