/*  rwengine.h - Read-Write Engine
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
#ifndef __RWENGINE_H__
#define __RWENGINE_H__

#include <wdm.h>
#include "csqimpl.h"


/*-----------------------------------------------------------------------------
 . Module Definitions
 -----------------------------------------------------------------------------*/


/* Timeout strategy definitions */
#define RWENGINE_TIMEOUT_BLOCK          0x00
#define RWENGINE_TIMEOUT_NONBLOCK       0x01
#define RWENGINE_TIMEOUT_TIME           0x02



typedef ULONG (*RwEngine_WriteToBuffer)(PVOID, PVOID, ULONG);
typedef ULONG (*RwEngine_ReadFromBuffer)(PVOID, PVOID, ULONG);



typedef struct _RWTIMEOUT
{
    ULONG ConstantTimeoutMs;
    ULONG MultiplierTimeoutMs;

} RWTIMEOUT, *PRWTIMEOUT;


typedef struct _RWTRANSFER
{
    PUCHAR buffer;
    ULONG bytesTransferred;
    ULONG transferLength;

} RWTRANSFER, *PRWTRANSFER;



typedef struct _RWENGINE
{
    BOOLEAN isEnabled;
    BOOLEAN readsArePending;
    BOOLEAN writesArePending;
    RWTRANSFER currentReadTransfer;
    RWTRANSFER currentWriteTransfer;

    CSQ readQueue;
    CSQ writeQueue;
    CSQ pendingReadQueue;
    CSQ pendingWriteQueue;

    UCHAR readTimeoutStrategy;
    UCHAR writeTimeoutStrategy;

    KDPC readTimeoutDpc;
    KDPC writeTimeoutDpc;
    KDPC writeBufferAvailableDpc;
    KDPC readDataAvailableDpc;

    KTIMER readTimeoutTimer;
    KTIMER writeTimeoutTimer;

    /* Locks */
    KSPIN_LOCK readLock;
    KSPIN_LOCK writeLock;

    /* Timeout values */
    RWTIMEOUT readTimeout;
    RWTIMEOUT writeTimeout;

    /* Buffer methods */
    PVOID userContext;
    RwEngine_WriteToBuffer WriteToBuffer;
    RwEngine_ReadFromBuffer ReadFromBuffer;

} RWENGINE, *PRWENGINE;




/*-----------------------------------------------------------------------------
 . Module Interface
 -----------------------------------------------------------------------------*/
VOID RwEngine_Initialize(
    PRWENGINE rwengine,
    RwEngine_WriteToBuffer writeCallback,
    RwEngine_ReadFromBuffer readCallback,
    PVOID callbackContext);

VOID RwEngine_Enable(PRWENGINE rwengine);
VOID RwEngine_Disable(PRWENGINE rwengine);

VOID RwEngine_SetReadTimeout(PRWENGINE rwengine, ULONG constantMs, ULONG multiplierMs);
VOID RwEngine_SetWriteTimeout(PRWENGINE rwengine, ULONG constantMs, ULONG multiplierMs);
VOID RwEngine_GetReadTimeout(PRWENGINE rwengine, ULONG* constantMs, ULONG* multiplierMs);
VOID RwEngine_GetWriteTimeout(PRWENGINE rwengine, ULONG* constantMs, ULONG* multiplierMs);

VOID RwEngine_SignalWriteBufferAvailable(PRWENGINE rwengine);
VOID RwEngine_SignalReadDataAvailable(PRWENGINE rwengine);

NTSTATUS RwEngine_DispatchRead(PRWENGINE rwengine, PIRP Irp);
NTSTATUS RwEngine_DispatchWrite(PRWENGINE rwengine, PIRP Irp);
VOID RwEngine_FlushQueuedAndPendingIrps(PRWENGINE rwengine);


#endif
