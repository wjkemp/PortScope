/*  pscontrol.h - Control Device Implementation
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
#ifndef __PSCONTROL_H__
#define __PSCONTROL_H__

#include "pscommon.h"
#include "psbuffer.h"
#include "rwengine.h"


/*-----------------------------------------------------------------------------
    Module Definitions
 -----------------------------------------------------------------------------*/

#define PORTSCOPE_BUFFER_SIZE 16384


typedef struct
{
    COMMON_DEVICE_DATA Common;
    BUFFER WriteBuffer;
    unsigned char WriteBufferData[PORTSCOPE_BUFFER_SIZE];

    BUFFER ReadBuffer;
    unsigned char ReadBufferData[PORTSCOPE_BUFFER_SIZE];

    ULONG TransmitDataTag;
    ULONG ReceiveDataTag;

    RWENGINE TransmitDataEngine;
    RWENGINE ReceiveDataEngine;

    LIST_ENTRY FilterDeviceList;   

} CONTROL_DEVICE_EXTENSION, *PCONTROL_DEVICE_EXTENSION;



/*-----------------------------------------------------------------------------
    Module Interface
 -----------------------------------------------------------------------------*/
NTSTATUS PortScope_ControlInitializeDeviceExtension(PCONTROL_DEVICE_EXTENSION deviceExtension);
VOID PortScope_ControlDestroyDeviceExtension(PCONTROL_DEVICE_EXTENSION deviceExtension);

NTSTATUS PortScope_ControlCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlRead(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlIoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlPnp(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlPower(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS PortScope_ControlUnknown(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS PortScope_InstallFilterDriver(PDEVICE_OBJECT ControlDevice, PUNICODE_STRING deviceName);



#endif
