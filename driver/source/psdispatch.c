/*  psdispatch.c - Main Dispatch Routines
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
#pragma alloc_text(PAGE, PortScope_DispatchCreate) 
#pragma alloc_text(PAGE, PortScope_DispatchClose) 
#pragma alloc_text(PAGE, PortScope_DispatchRead)
#pragma alloc_text(PAGE, PortScope_DispatchWrite)
#pragma alloc_text(PAGE, PortScope_DispatchIoControl) 
#pragma alloc_text(PAGE, PortScope_DispatchPnp)
#pragma alloc_text(PAGE, PortScope_DispatchPower)
#pragma alloc_text(PAGE, PortScope_DispatchUnknown)


/*---------------------------------------------------------------------------
    Variables
 ----------------------------------------------------------------------------*/ 


/*---------------------------------------------------------------------------
    Functions
 ----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PCOMMON_DEVICE_DATA commonDeviceData;

    PAGED_CODE();
            
    DBG0(("PortScope: DispatchCreate\n"));
    
    /* Dispatch */
    commonDeviceData = (PCOMMON_DEVICE_DATA)DeviceObject->DeviceExtension;
    switch (commonDeviceData->Type) {
        case DEVICE_TYPE_CONTROL: status = PortScope_ControlCreate(DeviceObject, Irp); break;
        case DEVICE_TYPE_FILTER: status = PortScope_FilterCreate(DeviceObject, Irp); break;
    }    

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PCOMMON_DEVICE_DATA commonDeviceData;

    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG0(("PortScope: DispatchClose\n"));

    /* Dispatch */
    commonDeviceData = (PCOMMON_DEVICE_DATA)DeviceObject->DeviceExtension;
    switch (commonDeviceData->Type) {
        case DEVICE_TYPE_CONTROL: status = PortScope_ControlClose(DeviceObject, Irp); break;
        case DEVICE_TYPE_FILTER: status = PortScope_FilterClose(DeviceObject, Irp); break;
    }    


    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_DispatchRead(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PCOMMON_DEVICE_DATA commonDeviceData;

    PAGED_CODE();
    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG0(("PortScope: DispatchRead\n"));

    /* Dispatch */
    commonDeviceData = (PCOMMON_DEVICE_DATA)DeviceObject->DeviceExtension;
    switch (commonDeviceData->Type) {
        case DEVICE_TYPE_CONTROL: status = PortScope_ControlRead(DeviceObject, Irp); break;
        case DEVICE_TYPE_FILTER: status = PortScope_FilterRead(DeviceObject, Irp); break;
    }    

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_DispatchWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PCOMMON_DEVICE_DATA commonDeviceData;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(DeviceObject);

    
    DBG0(("PortScope: DispatchWrite\n"));
    

    /* Dispatch */
    commonDeviceData = (PCOMMON_DEVICE_DATA)DeviceObject->DeviceExtension;
    switch (commonDeviceData->Type) {
        case DEVICE_TYPE_CONTROL: status = PortScope_ControlWrite(DeviceObject, Irp); break;
        case DEVICE_TYPE_FILTER: status = PortScope_FilterWrite(DeviceObject, Irp); break;
    }    

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_DispatchIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PCOMMON_DEVICE_DATA commonDeviceData;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG0(("PortScope: DispatchIoControl\n"));

    /* Dispatch */
    commonDeviceData = (PCOMMON_DEVICE_DATA)DeviceObject->DeviceExtension;
    switch (commonDeviceData->Type) {
        case DEVICE_TYPE_CONTROL: status = PortScope_ControlIoControl(DeviceObject, Irp); break;
        case DEVICE_TYPE_FILTER: status = PortScope_FilterIoControl(DeviceObject, Irp); break;
    }    


    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_DispatchPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PCOMMON_DEVICE_DATA commonDeviceData;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG0(("PortScope: DispatchPnp\n"));

    /* Dispatch */
    commonDeviceData = (PCOMMON_DEVICE_DATA)DeviceObject->DeviceExtension;
    switch (commonDeviceData->Type) {
        case DEVICE_TYPE_CONTROL: status = PortScope_ControlPnp(DeviceObject, Irp); break;
        case DEVICE_TYPE_FILTER: status = PortScope_FilterPnp(DeviceObject, Irp); break;
    }    

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_DispatchPower(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PCOMMON_DEVICE_DATA commonDeviceData;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG0(("PortScope: DispatchPower\n"));

    /* Dispatch */
    commonDeviceData = (PCOMMON_DEVICE_DATA)DeviceObject->DeviceExtension;
    switch (commonDeviceData->Type) {
        case DEVICE_TYPE_CONTROL: status = PortScope_ControlPower(DeviceObject, Irp); break;
        case DEVICE_TYPE_FILTER: status = PortScope_FilterPower(DeviceObject, Irp); break;
    }    

    return status;
}


/*---------------------------------------------------------------------------*/
NTSTATUS PortScope_DispatchUnknown(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PCOMMON_DEVICE_DATA commonDeviceData;

    PAGED_CODE();

    
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
        
    DBG0(("PortScope: DispatchUnknown\n"));

    /* Dispatch */
    commonDeviceData = (PCOMMON_DEVICE_DATA)DeviceObject->DeviceExtension;
    switch (commonDeviceData->Type) {
        case DEVICE_TYPE_CONTROL: status = PortScope_ControlUnknown(DeviceObject, Irp); break;
        case DEVICE_TYPE_FILTER: status = PortScope_FilterUnknown(DeviceObject, Irp); break;
    }    

    return status;
}
