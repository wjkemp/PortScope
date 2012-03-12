#include <ntifs.h>
#include <wdm.h>
#include <ntddk.h>
#include "portscope.h"

#pragma alloc_text(PAGE, PortScope_ControlCreate) 
#pragma alloc_text(PAGE, PortScope_ControlClose) 
#pragma alloc_text(PAGE, PortScope_ControlRead) 
#pragma alloc_text(PAGE, PortScope_ControlWrite)
#pragma alloc_text(PAGE, PortScope_ControlIoControl)
#pragma alloc_text(PAGE, PortScope_ControlPnp)
#pragma alloc_text(PAGE, PortScope_ControlPower)
#pragma alloc_text(PAGE, PortScope_ControlUnknown)




/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_ControlCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION stack;
    PCONTROL_DEVICE_EXTENSION deviceExtension;
    UNICODE_STRING deviceName;

    PAGED_CODE();
    
        
    DBG0(("PortScope: ControlCreate\n"));


    deviceExtension = (PCONTROL_DEVICE_EXTENSION) DeviceObject->DeviceExtension;
    stack = IoGetCurrentIrpStackLocation(Irp);


    // Check the device name
    RtlInitUnicodeString(&deviceName, L"\\transmit");
    if (RtlCompareUnicodeString(&stack->FileObject->FileName, &deviceName, TRUE) == 0) {
        DBG1(("PortScope: Opening Control Handle for Transmit Data\n"));
        stack->FileObject->FsContext = &deviceExtension->TransmitDataTag;

    } else {
        DBG1(("PortScope: Opening Control Handle for Receive Data\n"));
        stack->FileObject->FsContext = &deviceExtension->ReceiveDataTag;
    }

    // Complete the IRP
    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_ControlClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
        
    DBG0(("PortScope: ControlClose\n"));

    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_ControlRead(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpStack;
    PCONTROL_DEVICE_EXTENSION deviceExtension;
    
    PAGED_CODE();
        
    DBG0(("PortScope: ControlRead\n"));

    deviceExtension = (PCONTROL_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);


    /* Transmit Data read */
    if (irpStack->FileObject->FsContext == &deviceExtension->TransmitDataTag) {

        /* Dispatch to the transmit data engine */
        DBG1(("PortScope: Read Transmit Data\n"));
        status = RwEngine_DispatchRead(&deviceExtension->TransmitDataEngine, Irp);

    /* Receive Data Read */
    } else {

        /* Dispatch to the receive data engine */
        DBG1(("PortScope: Read Receive Data\n"));
        status = RwEngine_DispatchRead(&deviceExtension->ReceiveDataEngine, Irp);
    }

    return status;
}



/*---------------------------------------------------------------------------*/
ULONG PortScope_ReadTransmitData(PVOID context, PVOID buffer, ULONG length)
{
    PCONTROL_DEVICE_EXTENSION deviceExtension = (PCONTROL_DEVICE_EXTENSION)context;

    length = Buffer_Get(&deviceExtension->WriteBuffer, buffer, length);
    return length;
}


/*---------------------------------------------------------------------------*/
ULONG PortScope_ReadReceiveData(PVOID context, PVOID buffer, ULONG length)
{
    PCONTROL_DEVICE_EXTENSION deviceExtension = (PCONTROL_DEVICE_EXTENSION)context;

    length = Buffer_Get(&deviceExtension->ReadBuffer, buffer, length);
    return length;
}


/*---------------------------------------------------------------------------*/
ULONG PortScope_RwNullFunction(PVOID context, PVOID buffer, ULONG length)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(buffer);
    UNREFERENCED_PARAMETER(length);

    return 0;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_ControlWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
        
    DBG0(("PortScope: ControlWrite\n"));

    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_ControlIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PCONTROL_DEVICE_EXTENSION deviceExtension = (PCONTROL_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION  irpStack;

    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
        
    DBG0(("PortScope: ControlIoControl\n"));

    Irp->IoStatus.Information = 0;
    irpStack = IoGetCurrentIrpStackLocation(Irp);


    /* Switch on the control code */
    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {

        /* Open Port */
        case IOCTL_OPEN_PORT: {
            UNICODE_STRING deviceName;
            PLIST_ENTRY listEntry;
            PFILTER_DEVICE_EXTENSION filterEntry = 0;
            PCWSTR buffer = (PCWSTR)Irp->AssociatedIrp.SystemBuffer;
            ULONG length = irpStack->Parameters.DeviceIoControl.InputBufferLength;
            DbgPrint("PortScope: Length: %d\n", length);


            /* Initialize the device string */            
            RtlInitUnicodeString(&deviceName, buffer);
            DbgPrint("PortScope: Opening %wZ\n", &deviceName);

            /* Search the Filter Device List for an installed filter */
            listEntry = deviceExtension->FilterDeviceList.Flink;
            while (listEntry != &deviceExtension->FilterDeviceList) {
                PFILTER_DEVICE_EXTENSION filterDeviceExtension = 
                    CONTAINING_RECORD(listEntry, FILTER_DEVICE_EXTENSION, ListEntry);

                DbgPrint("PortScope: Checking %wZ\n", &filterDeviceExtension->DeviceName);
                if (RtlCompareUnicodeString(&deviceName, &filterDeviceExtension->DeviceName, TRUE) == 0) {
                    filterEntry = filterDeviceExtension;
                    break;
                }

                listEntry = listEntry->Flink;
            }

            if (filterEntry) {
                DBG1(("PortScope: Filter Device Object is already installed\n"));

            } else {
                DBG1(("PortScope: Installing filter driver\n"));
                PortScope_InstallFilterDriver(DeviceObject, &deviceName);
            }

        } break;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    return status;
}



/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_ControlPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG0(("PortScope: ControlPnp\n"));

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_ControlPower(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG0(("PortScope: ControlPower\n"));

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_ControlUnknown(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG0(("PortScope: ControlUnknown\n"));

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_InstallFilterDriver(PDEVICE_OBJECT ControlDevice, PUNICODE_STRING deviceName)
{
    NTSTATUS status = STATUS_SUCCESS;       
    PCONTROL_DEVICE_EXTENSION controlDeviceExtension = (PCONTROL_DEVICE_EXTENSION)ControlDevice->DeviceExtension;
    HANDLE fileHandle;
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    PFILE_OBJECT fileObject;
    PDEVICE_OBJECT remoteDeviceObject = NULL;

    DBG1(("PortScope: Installing filter driver\n"));
    
    /* Initialize the object attributes structure */
    InitializeObjectAttributes(
        &objectAttributes, 
        deviceName,
        OBJ_KERNEL_HANDLE,
        NULL,
        NULL);

    /* Attempt to open the file */                        
    status = ZwCreateFile(
        &fileHandle,
        GENERIC_READ,
        &objectAttributes,
        &ioStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN,
        0,
        NULL,
        0);

    if (NT_SUCCESS(status)) {
        
        /* Get the FILE_OBJECT from the object manager */
        status = ObReferenceObjectByHandle(
          fileHandle,
          READ_CONTROL,
          NULL,
          KernelMode,
          &fileObject,
          NULL);

        if (NT_SUCCESS(status)) {
        
            /* Save the device object pointer */
            remoteDeviceObject = fileObject->DeviceObject;
            
            /* Dereference the object */
            ObDereferenceObject(fileObject);        
        }
        
        /* Close the File */
        ZwClose(fileHandle);

    } else {
        DBG1(("PortScope: Could not open device file\n"));
    }
                      
                      
    /* If we have a successful state and a remote device object we can hook the filter driver */
    if (NT_SUCCESS(status) && (NULL != remoteDeviceObject)) {
        PDEVICE_OBJECT filterDeviceObject;

        /* Create the filter device */
        status = IoCreateDevice(
            ControlDevice->DriverObject,
            sizeof(FILTER_DEVICE_EXTENSION),
            NULL,
            FILE_DEVICE_UNKNOWN,
            FILE_DEVICE_SECURE_OPEN,
            FALSE,
            &filterDeviceObject);


        if (NT_SUCCESS(status)) {
            PFILTER_DEVICE_EXTENSION filterDeviceExtension;
            filterDeviceExtension = (PFILTER_DEVICE_EXTENSION) filterDeviceObject->DeviceExtension;
            filterDeviceExtension->Common.Type = DEVICE_TYPE_FILTER;
            
            /* Attach to remote device stack */
            filterDeviceExtension->NextLowerDriver = IoAttachDeviceToDeviceStack(
                filterDeviceObject,
                remoteDeviceObject);

            /* Successful attachment */
            if (NULL != filterDeviceExtension->NextLowerDriver) {

                DBG1(("PortScope: Filter driver was successfully installed\n"));

                /* Initialize the filter device object */
                filterDeviceObject->Flags |= filterDeviceExtension->NextLowerDriver->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);
                filterDeviceObject->DeviceType = filterDeviceExtension->NextLowerDriver->DeviceType;
                filterDeviceObject->Characteristics = filterDeviceExtension->NextLowerDriver->Characteristics;
                filterDeviceExtension->Self = filterDeviceObject;
                filterDeviceExtension->IrpsDispatched = 0;
                filterDeviceExtension->IrpsCompleted = 0;

                /* Store the device name */
                RtlCreateUnicodeString(
                    &filterDeviceExtension->DeviceName,
                    deviceName->Buffer);

                /* Initialize the remove lock */
                IoInitializeRemoveLock(
                    &filterDeviceExtension->RemoveLock, 
                    POOL_TAG,
                    1,
                    100);
            
                /* Initialize the PNP state */
                INITIALIZE_PNP_STATE(filterDeviceExtension);

                
                /* Clear the initialization flag */
                filterDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

                /* Back pointer to the control device extension */
                filterDeviceExtension->ControlDevice = controlDeviceExtension;

                /* Append the filter to the list */
                InsertTailList(&controlDeviceExtension->FilterDeviceList, &filterDeviceExtension->ListEntry);
                
            
            /* Failed to attach */
            } else {
                IoDeleteDevice(filterDeviceObject);                
            }
            
        }        
    }
    
    return status;
}

