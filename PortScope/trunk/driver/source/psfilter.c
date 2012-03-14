/*  psfilter.c - Filter Dispatch Functions
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
#include "psfilter.h"



/*---------------------------------------------------------------------------
    Defines
 ----------------------------------------------------------------------------*/
#pragma alloc_text(PAGE, PortScope_FilterCreate) 
#pragma alloc_text(PAGE, PortScope_FilterClose) 
#pragma alloc_text(PAGE, PortScope_FilterRead) 
#pragma alloc_text(PAGE, PortScope_FilterWrite)
#pragma alloc_text(PAGE, PortScope_FilterIoControl)
#pragma alloc_text(PAGE, PortScope_FilterPnp)
#pragma alloc_text(PAGE, PortScope_FilterPower)
#pragma alloc_text(PAGE, PortScope_FilterUnknown)

IO_COMPLETION_ROUTINE PortScope_FilterReadComplete;
IO_COMPLETION_ROUTINE FilterStartCompletionRoutine;
IO_COMPLETION_ROUTINE FilterDeviceUsageNotificationCompletionRoutine;


/*---------------------------------------------------------------------------
    Variables
 ----------------------------------------------------------------------------*/ 
 

/*---------------------------------------------------------------------------
    Functions
 ----------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterReadComplete(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context);
NTSTATUS FilterStartCompletionRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context);
NTSTATUS FilterDeviceUsageNotificationCompletionRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context);



/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PFILTER_DEVICE_EXTENSION deviceExtension = (PFILTER_DEVICE_EXTENSION) DeviceObject->DeviceExtension;

    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG2(("PortScope: FilterCreate\n"));


    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    IoSkipCurrentIrpStackLocation(Irp);
    status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);
    IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp);

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PFILTER_DEVICE_EXTENSION deviceExtension = (PFILTER_DEVICE_EXTENSION) DeviceObject->DeviceExtension;

    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG2(("PortScope: FilterClose\n"));

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    IoSkipCurrentIrpStackLocation(Irp);
    status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);
    IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp);

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterReadComplete(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{
    NTSTATUS status = STATUS_SUCCESS;
    PFILTER_DEVICE_EXTENSION deviceExtension = (PFILTER_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION irpStack;
    UCHAR* buffer;
    ULONG length;

    UNREFERENCED_PARAMETER(Context);

    DBG2(("PortScope: FilterReadComplete\n"));

    /* Get the current stack location */
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    /* Copy the data into the write buffer */
    if (deviceExtension->State == FILTER_ENABLED) {
        buffer = (PUCHAR)Irp->AssociatedIrp.SystemBuffer;
        length = (ULONG)Irp->IoStatus.Information;
        Buffer_Put(&deviceExtension->ControlDevice->ReadBuffer, buffer, length);
    }

    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterRead(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PFILTER_DEVICE_EXTENSION deviceExtension = (PFILTER_DEVICE_EXTENSION) DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION irpStack;

    PAGED_CODE();
            
    DBG2(("PortScope: FilterRead\n"));

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    deviceExtension->IrpsDispatched++;

    /* Get the current stack location */
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    IoCopyCurrentIrpStackLocationToNext(Irp);

    /* Set the Completion Routine */
    IoSetCompletionRoutine(Irp, PortScope_FilterReadComplete, deviceExtension, TRUE, TRUE, TRUE);

    /* Call down */
    status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);
    IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp);

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PFILTER_DEVICE_EXTENSION deviceExtension = (PFILTER_DEVICE_EXTENSION) DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION irpStack;
    UCHAR* buffer;
    ULONG length;

    PAGED_CODE();
    
        
    DBG2(("PortScope: FilterWrite\n"));

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    /* Get the current stack location */
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    /* Copy the data into the write buffer */
    if (deviceExtension->State == FILTER_ENABLED) {
        buffer = (PUCHAR)Irp->AssociatedIrp.SystemBuffer;
        length = irpStack->Parameters.Write.Length;
        Buffer_Put(&deviceExtension->ControlDevice->WriteBuffer, buffer, length);
    }

    /* Forward the IRP */
    IoSkipCurrentIrpStackLocation(Irp);
    status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);
    IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp);

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PFILTER_DEVICE_EXTENSION deviceExtension = (PFILTER_DEVICE_EXTENSION) DeviceObject->DeviceExtension;

    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG2(("PortScope: FilterIoControl\n"));

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    IoSkipCurrentIrpStackLocation(Irp);
    status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);
    IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp);

    return status;
}



/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    PFILTER_DEVICE_EXTENSION deviceExtension;
    PIO_STACK_LOCATION irpStack;
    NTSTATUS status;
    KEVENT event;

    PAGED_CODE();

    DBG2(("PortScope: FilterPnp\n"));

    deviceExtension = (PFILTER_DEVICE_EXTENSION) DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }
    

    switch (irpStack->MinorFunction) {
    case IRP_MN_START_DEVICE:

        // The device is starting.
        // We cannot touch the device (send it any non pnp irps) until a
        // start device has been passed down to the lower drivers.
        KeInitializeEvent(&event, NotificationEvent, FALSE);
        IoCopyCurrentIrpStackLocationToNext(Irp);
        IoSetCompletionRoutine(Irp,
                               FilterStartCompletionRoutine,
                               &event,
                               TRUE,
                               TRUE,
                               TRUE);

        status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);
        
        // Wait for lower drivers to be done with the Irp. Important thing to
        // note here is when you allocate memory for an event in the stack  
        // you must do a KernelMode wait instead of UserMode to prevent 
        // the stack from getting paged out.
        if (status == STATUS_PENDING) {

           KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);          
           status = Irp->IoStatus.Status;
        }


        if (NT_SUCCESS (status)) {

            // As we are successfully now back, we will
            // first set our state to Started.
            SET_NEW_PNP_STATE(deviceExtension, Started);

            // On the way up inherit FILE_REMOVABLE_MEDIA during Start.
            // This characteristic is available only after the driver stack is started!.
            if (deviceExtension->NextLowerDriver->Characteristics & FILE_REMOVABLE_MEDIA) {

                DeviceObject->Characteristics |= FILE_REMOVABLE_MEDIA;
            }
        }
        

        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp); 
        return status;

    case IRP_MN_REMOVE_DEVICE:

        // Wait for all outstanding requests to complete
        DBG1(("PortScope: Waiting for outstanding requests\n"));
        IoReleaseRemoveLockAndWait(&deviceExtension->RemoveLock, Irp);

        IoSkipCurrentIrpStackLocation(Irp);

        status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);

        SET_NEW_PNP_STATE(deviceExtension, Deleted);
        
        IoDetachDevice(deviceExtension->NextLowerDriver);
        IoDeleteDevice(DeviceObject);
        return status;


    case IRP_MN_QUERY_STOP_DEVICE:
        SET_NEW_PNP_STATE(deviceExtension, StopPending);
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_STOP_DEVICE:

        // Check to see whether you have received cancel-stop
        // without first receiving a query-stop. This could happen if someone
        // above us fails a query-stop and passes down the subsequent
        // cancel-stop.
        if (StopPending == deviceExtension->DevicePnPState) {

            // We did receive a query-stop, so restore.
            RESTORE_PREVIOUS_PNP_STATE(deviceExtension);
        }
        status = STATUS_SUCCESS; // We must not fail this IRP.
        break;

    case IRP_MN_STOP_DEVICE:
        SET_NEW_PNP_STATE(deviceExtension, Stopped);
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_REMOVE_DEVICE:

        SET_NEW_PNP_STATE(deviceExtension, RemovePending);
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_SURPRISE_REMOVAL:

        SET_NEW_PNP_STATE(deviceExtension, SurpriseRemovePending);
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_REMOVE_DEVICE:

        // Check to see whether you have received cancel-remove
        // without first receiving a query-remove. This could happen if
        // someone above us fails a query-remove and passes down the
        // subsequent cancel-remove.
        if (RemovePending == deviceExtension->DevicePnPState) {

            // We did receive a query-remove, so restore.
            RESTORE_PREVIOUS_PNP_STATE(deviceExtension);
        }

        status = STATUS_SUCCESS; // We must not fail this IRP.
        break;

    case IRP_MN_DEVICE_USAGE_NOTIFICATION:

        // On the way down, pagable might become set. Mimic the driver
        // above us. If no one is above us, just set pagable.
        #pragma prefast(suppress:__WARNING_INACCESSIBLE_MEMBER)
        if ((DeviceObject->AttachedDevice == NULL) ||
            (DeviceObject->AttachedDevice->Flags & DO_POWER_PAGABLE)) {

            DeviceObject->Flags |= DO_POWER_PAGABLE;
        }

        IoCopyCurrentIrpStackLocationToNext(Irp);

        IoSetCompletionRoutine(
            Irp,
            FilterDeviceUsageNotificationCompletionRoutine,
            NULL,
            TRUE,
            TRUE,
            TRUE
            );

        return IoCallDriver(deviceExtension->NextLowerDriver, Irp);

    default:

        // If you don't handle any IRP you must leave the
        // status as is.
        status = Irp->IoStatus.Status;

        break;
    }


    // Pass the IRP down and forget it.
    Irp->IoStatus.Status = status;
    IoSkipCurrentIrpStackLocation (Irp);
    status = IoCallDriver (deviceExtension->NextLowerDriver, Irp);
    IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp); 
    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS FilterStartCompletionRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{
    PKEVENT event = (PKEVENT)Context;

    UNREFERENCED_PARAMETER (DeviceObject);

    // If the lower driver didn't return STATUS_PENDING, we don't need to 
    // set the event because we won't be waiting on it. 
    // This optimization avoids grabbing the dispatcher lock, and improves perf.
    if (Irp->PendingReturned == TRUE) {
        KeSetEvent (event, IO_NO_INCREMENT, FALSE);
    }

    // The dispatch routine will have to call IoCompleteRequest
    return STATUS_MORE_PROCESSING_REQUIRED;

}


/*---------------------------------------------------------------------------*/
NTSTATUS FilterDeviceUsageNotificationCompletionRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{
    PFILTER_DEVICE_EXTENSION deviceExtension;

    UNREFERENCED_PARAMETER(Context);

    deviceExtension = (PFILTER_DEVICE_EXTENSION) DeviceObject->DeviceExtension;

    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    // On the way up, pagable might become clear. Mimic the driver below us.
    if (!(deviceExtension->NextLowerDriver->Flags & DO_POWER_PAGABLE)) {
        DeviceObject->Flags &= ~DO_POWER_PAGABLE;
    }


    IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp); 

    return STATUS_CONTINUE_COMPLETION;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterPower(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    PFILTER_DEVICE_EXTENSION deviceExtension;
    NTSTATUS status;

    PAGED_CODE();
    
    deviceExtension = (PFILTER_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) { // may be device is being removed.
        Irp->IoStatus.Status = status;
        PoStartNextPowerIrp(Irp);
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    PoStartNextPowerIrp(Irp);
    IoSkipCurrentIrpStackLocation(Irp);
    status = PoCallDriver(deviceExtension->NextLowerDriver, Irp);
    IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp); 
    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterUnknown(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PFILTER_DEVICE_EXTENSION deviceExtension = (PFILTER_DEVICE_EXTENSION) DeviceObject->DeviceExtension;

    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG2(("PortScope: FilterUnknown\n"));

    status = IoAcquireRemoveLock (&deviceExtension->RemoveLock, Irp);
    if (!NT_SUCCESS (status)) {
        Irp->IoStatus.Status = status;
        IoCompleteRequest (Irp, IO_NO_INCREMENT);
        return status;
    }

    IoSkipCurrentIrpStackLocation(Irp);
    status = IoCallDriver(deviceExtension->NextLowerDriver, Irp);
    IoReleaseRemoveLock(&deviceExtension->RemoveLock, Irp);

    return status;
}

