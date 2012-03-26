/*  psdispatch.h - Main Dispatch Routines
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
#ifndef __PSDISPATCH_H__
#define __PSDISPATCH_H__

#include "pscommon.h"
#include "rwengine.h"


/*-----------------------------------------------------------------------------
    Module Definitions
 -----------------------------------------------------------------------------*/

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

DRIVER_DISPATCH PortScope_DispatchUnknown;



/*-----------------------------------------------------------------------------
    Module Interface
 -----------------------------------------------------------------------------*/
NTSTATUS PortScope_DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchRead(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchPower(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_DispatchUnknown(PDEVICE_OBJECT DeviceObject, PIRP Irp);




#endif
