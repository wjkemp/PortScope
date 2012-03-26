/*  psfilter.h - Filter Dispatch Functions
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
#ifndef __PSFILTER_H__
#define __PSFILTER_H__

#include "pscommon.h"
#include "pscontrol.h"


/*-----------------------------------------------------------------------------
    Module Definitions
 -----------------------------------------------------------------------------*/


typedef enum _FILTER_STATE {

    FILTER_DISABLED = 0,
    FILTER_ENABLED,
    FILTER_ABANDONED

} FILTER_STATE;



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
    FILTER_STATE State;
    LIST_ENTRY ListEntry;   

} FILTER_DEVICE_EXTENSION, *PFILTER_DEVICE_EXTENSION;


/*-----------------------------------------------------------------------------
    Module Interface
 -----------------------------------------------------------------------------*/
NTSTATUS PortScope_FilterCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterRead(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterPower(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_FilterUnknown(PDEVICE_OBJECT DeviceObject, PIRP Irp);


#endif
