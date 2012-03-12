/*  rwengine.c - Read-Write Engine
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
#include "rwengine.h"



/*---------------------------------------------------------------------------
    Defines
 ----------------------------------------------------------------------------*/
KDEFERRED_ROUTINE RwEngine_ReadDataAvailableDPC;
KDEFERRED_ROUTINE RwEngine_ReadTimeoutDPC;
KDEFERRED_ROUTINE RwEngine_WriteBufferAvailableDPC;
KDEFERRED_ROUTINE RwEngine_WriteTimeoutDPC;


/*---------------------------------------------------------------------------
    Variables
 ----------------------------------------------------------------------------*/ 
 
/*---------------------------------------------------------------------------
    Functions
 ----------------------------------------------------------------------------*/
VOID RwEngine_ReadDataAvailableDPC(PKDPC Dpc, PVOID context, PVOID arg1, PVOID arg2);
VOID RwEngine_ReadTimeoutDPC(PKDPC Dpc, PVOID context, PVOID arg1, PVOID arg2);
static VOID RwEngine_OnCurrentReadCancelled(PVOID context, PIRP Irp);
static VOID RwEngine_EnqueueAvailableReadIrps(PRWENGINE rwengine);
static VOID RwEngine_StartReadTimeoutTimer(PRWENGINE rwengine);

VOID RwEngine_WriteBufferAvailableDPC(PKDPC Dpc, PVOID context, PVOID arg1, PVOID arg2);
VOID RwEngine_WriteTimeoutDPC(PKDPC Dpc, PVOID context, PVOID arg1, PVOID arg2);
static VOID RwEngine_OnCurrentWriteCancelled(PVOID context, PIRP Irp);
static VOID RwEngine_EnqueueAvailableWriteIrps(PRWENGINE rwengine);
static VOID RwEngine_StartWriteTimeoutTimer(PRWENGINE rwengine);



/*----------------------------------------------------------------------------*/
VOID RwEngine_Initialize(
    PRWENGINE rwengine,
    RwEngine_WriteToBuffer writeCallback,
    RwEngine_ReadFromBuffer readCallback,
    PVOID callbackContext)
{
    /* Reset the flags */
    rwengine->isEnabled = FALSE;
    rwengine->readsArePending = FALSE;
    rwengine->writesArePending = FALSE;

    /* Save the callbacks */
    rwengine->ReadFromBuffer = readCallback;
    rwengine->WriteToBuffer = writeCallback;
    rwengine->userContext = callbackContext;

    /* Clear out the transfer objects */
    rwengine->currentReadTransfer.buffer = NULL;
    rwengine->currentReadTransfer.transferLength = 0;
    rwengine->currentReadTransfer.bytesTransferred = 0;

    rwengine->currentWriteTransfer.buffer = NULL;
    rwengine->currentWriteTransfer.transferLength = 0;
    rwengine->currentWriteTransfer.bytesTransferred = 0;

    /* Set the timeout strategies to blocking */
    rwengine->readTimeoutStrategy = RWENGINE_TIMEOUT_BLOCK;
    rwengine->writeTimeoutStrategy = RWENGINE_TIMEOUT_BLOCK;

    /* Reset the timeout values */
    rwengine->readTimeout.ConstantTimeoutMs = 0xFFFFFFFF;
    rwengine->readTimeout.MultiplierTimeoutMs = 0;
    rwengine->writeTimeout.ConstantTimeoutMs = 0xFFFFFFFF;
    rwengine->writeTimeout.MultiplierTimeoutMs = 0;


    /* Cancel-safe queues */
    CsqInitialize(&rwengine->readQueue, NULL, NULL);
    CsqInitialize(&rwengine->writeQueue, NULL, NULL);
    CsqInitialize(&rwengine->pendingReadQueue, RwEngine_OnCurrentReadCancelled, rwengine);
    CsqInitialize(&rwengine->pendingWriteQueue, RwEngine_OnCurrentWriteCancelled, rwengine);

    /* DPC objects */
    KeInitializeDpc(&rwengine->writeBufferAvailableDpc, RwEngine_WriteBufferAvailableDPC, rwengine);
    KeInitializeDpc(&rwengine->readDataAvailableDpc, RwEngine_ReadDataAvailableDPC, rwengine);
    KeInitializeDpc(&rwengine->writeTimeoutDpc, RwEngine_WriteTimeoutDPC, rwengine);
    KeInitializeDpc(&rwengine->readTimeoutDpc, RwEngine_ReadTimeoutDPC, rwengine);

    /* Timers */
    KeInitializeTimer(&rwengine->writeTimeoutTimer);
    KeInitializeTimer(&rwengine->readTimeoutTimer);

}


/*----------------------------------------------------------------------------*/
VOID RwEngine_Enable(PRWENGINE rwengine)
{
    KIRQL oldirql;

    /* Synchronize */
    KeRaiseIrql(DISPATCH_LEVEL, &oldirql);

    /* Indicate that the engine is enabled */
    rwengine->isEnabled = TRUE;

    KeLowerIrql(oldirql);   
}


/*----------------------------------------------------------------------------*/
VOID RwEngine_Disable(PRWENGINE rwengine)
{
    KIRQL oldirql;

    /* Synchronize */
    KeRaiseIrql(DISPATCH_LEVEL, &oldirql);

    /* Cancel timeout timers, just to be safe */
    KeCancelTimer(&rwengine->writeTimeoutTimer);
    KeCancelTimer(&rwengine->readTimeoutTimer);

    /* Flush all pending IRPs */
    RwEngine_FlushQueuedAndPendingIrps(rwengine);

    /* Indicate that the engine is disabled */
    rwengine->isEnabled = FALSE;

    KeLowerIrql(oldirql);   
}


/*=============================================================================
    Properties
 =============================================================================*/


/*----------------------------------------------------------------------------*/
VOID RwEngine_SetReadTimeout(PRWENGINE rwengine, ULONG constantMs, ULONG multiplierMs)
{
    KIRQL oldirql;
    KeRaiseIrql(DISPATCH_LEVEL, &oldirql);

    /* Set the timeouts */
    rwengine->readTimeout.ConstantTimeoutMs = constantMs;
    rwengine->readTimeout.MultiplierTimeoutMs = multiplierMs;

    /* Update the strategy */
    if (constantMs == 0xFFFFFFFF) {
        rwengine->readTimeoutStrategy = RWENGINE_TIMEOUT_BLOCK;

    } else if ((constantMs == 0) && (multiplierMs == 0)) {
        rwengine->readTimeoutStrategy = RWENGINE_TIMEOUT_NONBLOCK;

    } else {
        rwengine->readTimeoutStrategy = RWENGINE_TIMEOUT_TIME;
    }


    KeLowerIrql(oldirql);
}


/*----------------------------------------------------------------------------*/
VOID RwEngine_SetWriteTimeout(PRWENGINE rwengine, ULONG constantMs, ULONG multiplierMs)
{
    KIRQL oldirql;
    KeRaiseIrql(DISPATCH_LEVEL, &oldirql);

    /* Set the timeouts */
    rwengine->writeTimeout.ConstantTimeoutMs = constantMs;
    rwengine->writeTimeout.MultiplierTimeoutMs = multiplierMs;

    /* Update the strategy */
    if (constantMs == 0xFFFFFFFF) {
        rwengine->writeTimeoutStrategy = RWENGINE_TIMEOUT_BLOCK;

    } else if ((constantMs == 0) && (multiplierMs == 0)) {
        rwengine->writeTimeoutStrategy = RWENGINE_TIMEOUT_NONBLOCK;

    } else {
        rwengine->writeTimeoutStrategy = RWENGINE_TIMEOUT_TIME;
    }


    KeLowerIrql(oldirql);
}


/*----------------------------------------------------------------------------*/
VOID RwEngine_GetReadTimeout(PRWENGINE rwengine, ULONG* constantMs, ULONG* multiplierMs)
{
    *constantMs = rwengine->readTimeout.ConstantTimeoutMs;
    *multiplierMs = rwengine->readTimeout.MultiplierTimeoutMs;
}


/*----------------------------------------------------------------------------*/
VOID RwEngine_GetWriteTimeout(PRWENGINE rwengine, ULONG* constantMs, ULONG* multiplierMs)
{
    *constantMs = rwengine->writeTimeout.ConstantTimeoutMs;
    *multiplierMs = rwengine->writeTimeout.MultiplierTimeoutMs;
}



/*=============================================================================
    Get Methods
 =============================================================================*/


/*----------------------------------------------------------------------------*/
VOID RwEngine_SignalWriteBufferAvailable(PRWENGINE rwengine)
{
    KeInsertQueueDpc(&rwengine->writeBufferAvailableDpc, NULL, NULL);
}


/*----------------------------------------------------------------------------*/
VOID RwEngine_SignalReadDataAvailable(PRWENGINE rwengine)
{
    KeInsertQueueDpc(&rwengine->readDataAvailableDpc, NULL, NULL);
}


/*=============================================================================
    Read Engine
 =============================================================================*/


/*----------------------------------------------------------------------------*/
NTSTATUS RwEngine_DispatchRead(PRWENGINE rwengine, PIRP Irp)
{
    PIO_STACK_LOCATION stack;
    KIRQL oldirql;
    ULONG bytesRead;


    /* Get the current stack location */
    stack = IoGetCurrentIrpStackLocation(Irp);

    /* Short-circuit 0 length reads */
    if (stack->Parameters.Read.Length == 0) {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }
    
    /* Raise to dispatch level */
    KeRaiseIrql(DISPATCH_LEVEL, &oldirql);


    /* Politely refuse to process requests if we are disabled */
    if (rwengine->isEnabled == FALSE) {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        KeLowerIrql(oldirql);
        return STATUS_SUCCESS;
    }


    /* If a read is pending, simply enqueue this read */
    if (rwengine->readsArePending) {

        /* IoCsqInsertIrp will mark it as pending for us */
        IoCsqInsertIrp(&rwengine->readQueue.csq, Irp, NULL);


    /* If no reads are pending, process this read */
    } else {

        /* Fill out the read transfer */
        rwengine->currentReadTransfer.buffer = (PUCHAR)Irp->AssociatedIrp.SystemBuffer;
        rwengine->currentReadTransfer.transferLength = stack->Parameters.Read.Length;
        rwengine->currentReadTransfer.bytesTransferred = 0;

        /* Attempt to read the maximum amount of bytes from the buffer */
        bytesRead = rwengine->ReadFromBuffer(
            rwengine->userContext,
            rwengine->currentReadTransfer.buffer,
            rwengine->currentReadTransfer.transferLength);

        /* Adjust the transfer object */
        rwengine->currentReadTransfer.buffer += bytesRead;
        rwengine->currentReadTransfer.bytesTransferred += bytesRead;

        /* If we are done, or if the timeout strategy is non-blocking, complete the request */
        if ((rwengine->currentReadTransfer.bytesTransferred == rwengine->currentReadTransfer.transferLength) ||
            (rwengine->readTimeoutStrategy == RWENGINE_TIMEOUT_NONBLOCK)) {

            /* Complete */
            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = rwengine->currentReadTransfer.bytesTransferred;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            KeLowerIrql(oldirql);

            /* Clear out the transfer object for debugging purposes */
            rwengine->currentReadTransfer.buffer = NULL;
            rwengine->currentReadTransfer.transferLength = 0;
            rwengine->currentReadTransfer.bytesTransferred = 0;

            return STATUS_SUCCESS;

        /* Else enqueue for further processing */
        } else {

            IoCsqInsertIrp(&rwengine->pendingReadQueue.csq, Irp, NULL);

            /* If needed, start the timeout timer */
            if (rwengine->readTimeoutStrategy == RWENGINE_TIMEOUT_TIME) {
                RwEngine_StartReadTimeoutTimer(rwengine);
            }

            /* Indicate that transfers are queued */
            rwengine->readsArePending = TRUE;
        }
    }

    KeLowerIrql(oldirql);
    return STATUS_PENDING;
}


/*----------------------------------------------------------------------------*/
VOID RwEngine_ReadDataAvailableDPC(PKDPC Dpc, PVOID context, PVOID arg1, PVOID arg2)
{
    PIRP pendingIrp;
    PRWENGINE rwengine;
    ULONG bytesRead;

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(arg1);
    UNREFERENCED_PARAMETER(arg2);

    /* Get a pointer to the engine */
    rwengine = (PRWENGINE)context;


    /* If there is a read pending, handle it */
    if (rwengine->readsArePending) {

        /* Remove from the parked queue */
        pendingIrp = IoCsqRemoveNextIrp(&rwengine->pendingReadQueue.csq, NULL);   

        /* If the Irp is present, process it */
        if (pendingIrp) {

            /* Read as much as possible */
            bytesRead = rwengine->ReadFromBuffer(
                rwengine->userContext,
                rwengine->currentReadTransfer.buffer,
                rwengine->currentReadTransfer.transferLength -
                rwengine->currentReadTransfer.bytesTransferred);

            /* Adjust the transfer object */
            rwengine->currentReadTransfer.buffer += bytesRead;
            rwengine->currentReadTransfer.bytesTransferred += bytesRead;

            /* Complete the request if the write was satisfied. */
            if (rwengine->currentReadTransfer.bytesTransferred == rwengine->currentReadTransfer.transferLength) {

                /* Cancel the timeout timer */
                KeCancelTimer(&rwengine->readTimeoutTimer);

                /* Complete the IRP */
                pendingIrp->IoStatus.Status = STATUS_SUCCESS;
                pendingIrp->IoStatus.Information = rwengine->currentReadTransfer.bytesTransferred;
                IoCompleteRequest(pendingIrp, IO_SERIAL_INCREMENT);

                /* Clear out the transfer object for debugging purposes */
                rwengine->currentReadTransfer.buffer = NULL;
                rwengine->currentReadTransfer.transferLength = 0;
                rwengine->currentReadTransfer.bytesTransferred = 0;

                /* Push requests in the waiting queue until they are all done, or one blocks */
                RwEngine_EnqueueAvailableReadIrps(rwengine);

            /* If the read was not satisfied, it goes back on the parked queue */
            } else {
                IoCsqInsertIrp(&rwengine->pendingReadQueue.csq, pendingIrp, NULL);
            }

        /* If we get here, the driver for some reason thinks that there is a request pending,
           when in fact there is not. Push requests in the waiting queue. This will prevent
           a stall, and ensure that the queue busy flag is cleared when there are no more requests */
        } else {
            RwEngine_EnqueueAvailableReadIrps(rwengine);
        }
    }
}


/*----------------------------------------------------------------------------*/
VOID RwEngine_ReadTimeoutDPC(PKDPC Dpc, PVOID context, PVOID arg1, PVOID arg2)
{
    PIRP pendingIrp;
    PRWENGINE rwengine;

    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(arg1);
    UNREFERENCED_PARAMETER(arg2);

    /* Get a pointer to the engine */
    rwengine = (PRWENGINE)context;

    /* If there is a read pending, handle it */
    if (rwengine->readsArePending) {

        /* remove from the parked queue */
        pendingIrp = IoCsqRemoveNextIrp(&rwengine->pendingReadQueue.csq, NULL);   

        /* If there actually is an IRP, cancel it */
        if (pendingIrp) {

            /* Cancel the timeout timer. Technically it will only expire once,
               and this is it, but better to be safe */
            KeCancelTimer(&rwengine->readTimeoutTimer);

            /* Complete the current IRP */
            pendingIrp->IoStatus.Status = STATUS_SUCCESS;
            pendingIrp->IoStatus.Information = rwengine->currentReadTransfer.bytesTransferred;
            IoCompleteRequest(pendingIrp, IO_SERIAL_INCREMENT);

            /* Clear out the transfer object for debugging purposes */
            rwengine->currentReadTransfer.buffer = NULL;
            rwengine->currentReadTransfer.transferLength = 0;
            rwengine->currentReadTransfer.bytesTransferred = 0;
        }

        /* Push any requests that may be in the waiting queue */
        RwEngine_EnqueueAvailableReadIrps(rwengine);


    /* If we get here, the timer timed out but no current write is present.
       This should never happen, but if it does, make sure that the timer is
       cancelled */
    } else {
        KeCancelTimer(&rwengine->readTimeoutTimer);
    }
}


/*----------------------------------------------------------------------------*/
static VOID RwEngine_OnCurrentReadCancelled(PVOID context, PIRP Irp)
{
    KIRQL oldirql;
    PRWENGINE rwengine = (PRWENGINE)context;

    /* Synchronize */
    KeRaiseIrql(DISPATCH_LEVEL, &oldirql);

    /* Cancel the timeout timer that may be associated with this Irp */
    KeCancelTimer(&rwengine->readTimeoutTimer);

    /* Complete the Irp */
    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    /* Clear out the transfer object for debugging purposes */
    rwengine->currentReadTransfer.buffer = NULL;
    rwengine->currentReadTransfer.transferLength = 0;
    rwengine->currentReadTransfer.bytesTransferred = 0;
    
    /* Push any requests that may be in the waiting queue */
    RwEngine_EnqueueAvailableReadIrps(rwengine);

    KeLowerIrql(oldirql);
}


/*----------------------------------------------------------------------------*/
static VOID RwEngine_EnqueueAvailableReadIrps(PRWENGINE rwengine)
{
    PIRP nextIrp;
    PIO_STACK_LOCATION stack;
    ULONG bytesRead;

    /* Check the queue until no irps remain */
    do {

        /* Get queued Irp */
        nextIrp = IoCsqRemoveNextIrp(&rwengine->readQueue.csq, NULL);
        if (nextIrp) {

            stack = IoGetCurrentIrpStackLocation(nextIrp);

            /* Fill out the write transfer */
            rwengine->currentReadTransfer.buffer = (PUCHAR)nextIrp->AssociatedIrp.SystemBuffer;
            rwengine->currentReadTransfer.transferLength = stack->Parameters.Read.Length;
            rwengine->currentReadTransfer.bytesTransferred = 0;

            /* Attempt to write the maximum amount of bytes into the buffer */
            bytesRead = rwengine->ReadFromBuffer(
                rwengine->userContext,
                rwengine->currentReadTransfer.buffer,
                rwengine->currentReadTransfer.transferLength);

            /* Adjust the transfer object */
            rwengine->currentReadTransfer.buffer += bytesRead;
            rwengine->currentReadTransfer.bytesTransferred += bytesRead;

            if (rwengine->currentReadTransfer.bytesTransferred == rwengine->currentReadTransfer.transferLength) {

                /* Complete */
                nextIrp->IoStatus.Status = STATUS_SUCCESS;
                nextIrp->IoStatus.Information = rwengine->currentReadTransfer.bytesTransferred;
                IoCompleteRequest(nextIrp, IO_SERIAL_INCREMENT);

            } else {

                /* Pend */
                IoCsqInsertIrp(&rwengine->pendingReadQueue.csq, nextIrp, NULL);

                /* If needed, start the timeout timer */
                if (rwengine->readTimeoutStrategy == RWENGINE_TIMEOUT_TIME) {
                    RwEngine_StartReadTimeoutTimer(rwengine);
                }

                return;
            }
        }

    } while (nextIrp != NULL);


    /* If we get here, all the queued requests were completed */
    rwengine->readsArePending = FALSE;
}



/*----------------------------------------------------------------------------*/
static VOID RwEngine_StartReadTimeoutTimer(PRWENGINE rwengine)
{
    LARGE_INTEGER timeout;

    /* Calculate the timeout */
    timeout.QuadPart =
        ((ULONGLONG)rwengine->readTimeout.ConstantTimeoutMs +
        ((ULONGLONG)rwengine->readTimeout.MultiplierTimeoutMs *
        (ULONGLONG)(rwengine->currentReadTransfer.transferLength - rwengine->currentReadTransfer.bytesTransferred))) 
            * -10000;
    
    /* Set the timer */
    KeSetTimer(&rwengine->readTimeoutTimer, timeout, &rwengine->readTimeoutDpc);
}



/*=============================================================================
    Write Engine
 =============================================================================*/


/*----------------------------------------------------------------------------*/
NTSTATUS RwEngine_DispatchWrite(PRWENGINE rwengine, PIRP Irp)
{
    PIO_STACK_LOCATION stack;
    KIRQL oldirql;
    NTSTATUS status;
    ULONG bytesWritten;


    /* Get the current stack location */
    stack = IoGetCurrentIrpStackLocation(Irp);

    /* Short-circuit 0 length writes */
    if (stack->Parameters.Write.Length == 0) {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }
    
    /* Raise to dispatch level */
    status = STATUS_SUCCESS;
    KeRaiseIrql(DISPATCH_LEVEL, &oldirql);


    /* Politely refuse to process requests if we are disabled */
    if (rwengine->isEnabled == FALSE) {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        KeLowerIrql(oldirql);
        return STATUS_SUCCESS;
    }


    /* If no requests are pending, start the transmission */
    if (rwengine->writesArePending == FALSE) {

        /* Fill out the write transfer */
        rwengine->currentWriteTransfer.buffer = (PUCHAR)Irp->AssociatedIrp.SystemBuffer;
        rwengine->currentWriteTransfer.transferLength = stack->Parameters.Write.Length;
        rwengine->currentWriteTransfer.bytesTransferred = 0;

        /* Attempt to write the maximum amount of bytes into the buffer */
        bytesWritten = rwengine->WriteToBuffer(
            rwengine->userContext,
            rwengine->currentWriteTransfer.buffer,
            rwengine->currentWriteTransfer.transferLength);

        /* Adjust the transfer object */
        rwengine->currentWriteTransfer.buffer += bytesWritten;
        rwengine->currentWriteTransfer.bytesTransferred += bytesWritten;

        /* If all the bytes were written, OR if the timeouts are set to nonblocking, we
           complete the request */
        if ((rwengine->currentWriteTransfer.bytesTransferred == rwengine->currentWriteTransfer.transferLength) ||
            (rwengine->writeTimeoutStrategy == RWENGINE_TIMEOUT_NONBLOCK)) {

            /* Complete the request */
            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = rwengine->currentWriteTransfer.bytesTransferred;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            status = STATUS_SUCCESS;

            /* Clear out the transfer object for debugging purposes */
            rwengine->currentWriteTransfer.buffer = NULL;
            rwengine->currentWriteTransfer.transferLength = 0;
            rwengine->currentWriteTransfer.bytesTransferred = 0;

        /* Else we pend the request */
        } else {
            rwengine->writesArePending = TRUE;
            IoMarkIrpPending(Irp);
            IoCsqInsertIrp(&rwengine->pendingWriteQueue.csq, Irp, NULL);

            /* If needed, start the timeout timer */
            if (rwengine->writeTimeoutStrategy == RWENGINE_TIMEOUT_TIME) {
                RwEngine_StartWriteTimeoutTimer(rwengine);
            }

            /* Indicate the status as pending */
            status = STATUS_PENDING;
        }


    /*  If there is a pending request, we enqueue the current request
        Timeouts are only set for the request at the head of the queue,
        any requests other than the current will have to be cancelled manually */
    } else {

        /* IoCsqInsertIrp will mark it as pending for us */
        IoCsqInsertIrp(&rwengine->writeQueue.csq, Irp, NULL);
        status = STATUS_PENDING;
    }

    
    KeLowerIrql(oldirql);
    return status;
}


/*----------------------------------------------------------------------------*/
VOID RwEngine_WriteBufferAvailableDPC(PKDPC Dpc, PVOID context, PVOID arg1, PVOID arg2)
{
    PIRP pendingIrp;
    PRWENGINE rwengine;
    ULONG bytesWritten;


    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(arg1);
    UNREFERENCED_PARAMETER(arg2);


    /* Get a pointer to the engine */
    rwengine = (PRWENGINE)context;


    /* If there is a write pending, handle it */
    if (rwengine->writesArePending) {

        /* Remove from the parked queue */
        pendingIrp = IoCsqRemoveNextIrp(&rwengine->pendingWriteQueue.csq, NULL);   

        /* If the Irp is present, process it */
        if (pendingIrp) {

            /* Attempt to write the remainder of bytes into the buffer */
            bytesWritten = rwengine->WriteToBuffer(
                rwengine->userContext,
                rwengine->currentWriteTransfer.buffer,
                rwengine->currentWriteTransfer.transferLength -
                rwengine->currentWriteTransfer.bytesTransferred);

            /* Adjust the transfer object */
            rwengine->currentWriteTransfer.buffer += bytesWritten;
            rwengine->currentWriteTransfer.bytesTransferred += bytesWritten;

            /* Complete the request if the write was satisfied. */
            if (rwengine->currentWriteTransfer.bytesTransferred == rwengine->currentWriteTransfer.transferLength) {

                /* Cancel the timeout timer */
                KeCancelTimer(&rwengine->writeTimeoutTimer);

                /* Complete the IRP */
                pendingIrp->IoStatus.Status = STATUS_SUCCESS;
                pendingIrp->IoStatus.Information = rwengine->currentWriteTransfer.bytesTransferred;
                IoCompleteRequest(pendingIrp, IO_SERIAL_INCREMENT);

                /* Clear out the transfer object for debugging purposes */
                rwengine->currentWriteTransfer.buffer = NULL;
                rwengine->currentWriteTransfer.transferLength = 0;
                rwengine->currentWriteTransfer.bytesTransferred = 0;


                /* Push requests in the waiting queue until they are all done, or one blocks */
                RwEngine_EnqueueAvailableWriteIrps(rwengine);

            /* If the write was not satisfied, it goes back on the parked queue */
            } else {
                IoCsqInsertIrp(&rwengine->pendingWriteQueue.csq, pendingIrp, NULL);
            }

        /* If we get here, the driver for some reason thinks that there is a request pending,
           when in fact there is not. Push requests in the waiting queue. This will prevent
           a stall, and ensure that the queue busy flag is cleared when there are no more requests */
        } else {
            RwEngine_EnqueueAvailableWriteIrps(rwengine);
        }
    }
}


/*----------------------------------------------------------------------------*/
VOID RwEngine_WriteTimeoutDPC(PKDPC Dpc, PVOID context, PVOID arg1, PVOID arg2)
{
    PIRP pendingIrp;
    PRWENGINE rwengine;


    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(arg1);
    UNREFERENCED_PARAMETER(arg2);


    /* Get a pointer to the engine */
    rwengine = (PRWENGINE)context;

    /* If there is a write pending, handle it */
    if (rwengine->writesArePending) {

        /* remove from the parked queue */
        pendingIrp = IoCsqRemoveNextIrp(&rwengine->pendingWriteQueue.csq, NULL);   

        /* If there actually is an IRP, cancel it */
        if (pendingIrp) {

            /* Cancel the timeout timer. Technically it will only expire once,
               and this is it, but better to be safe */
            KeCancelTimer(&rwengine->writeTimeoutTimer);

            /* Complete the current IRP */
            pendingIrp->IoStatus.Status = STATUS_SUCCESS;
            pendingIrp->IoStatus.Information = rwengine->currentWriteTransfer.bytesTransferred;
            IoCompleteRequest(pendingIrp, IO_SERIAL_INCREMENT);

            /* Clear out the transfer object for debugging purposes */
            rwengine->currentWriteTransfer.buffer = NULL;
            rwengine->currentWriteTransfer.transferLength = 0;
            rwengine->currentWriteTransfer.bytesTransferred = 0;
        }

        /* Push any requests that may be in the waiting queue */
        RwEngine_EnqueueAvailableWriteIrps(rwengine);


    /* If we get here, the timer timed out but no current write is present.
       This should never happen, but if it does, make sure that the timer is
       cancelled */
    } else {
        KeCancelTimer(&rwengine->writeTimeoutTimer);
    }
}

/*----------------------------------------------------------------------------*/
static VOID RwEngine_OnCurrentWriteCancelled(PVOID context, PIRP Irp)
{
    KIRQL oldirql;
    PRWENGINE rwengine = (PRWENGINE)context;

    /* Synchronize */
    KeRaiseIrql(DISPATCH_LEVEL, &oldirql);

    /* Cancel the timeout timer that may be associated with this Irp */
    KeCancelTimer(&rwengine->writeTimeoutTimer);

    /* Complete the Irp */
    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    /* Clear out the transfer object for debugging purposes */
    rwengine->currentWriteTransfer.buffer = NULL;
    rwengine->currentWriteTransfer.transferLength = 0;
    rwengine->currentWriteTransfer.bytesTransferred = 0;
    
    /* Push any requests that may be in the waiting queue */
    RwEngine_EnqueueAvailableWriteIrps(rwengine);

    KeLowerIrql(oldirql);   
}


/*----------------------------------------------------------------------------*/
static VOID RwEngine_EnqueueAvailableWriteIrps(PRWENGINE rwengine)
{
    PIRP nextIrp;
    PIO_STACK_LOCATION stack;
    ULONG bytesWritten;

    /* Check the queue until no irps remain */
    do {

        /* Get queued Irp */
        nextIrp = IoCsqRemoveNextIrp(&rwengine->writeQueue.csq, NULL);
        if (nextIrp) {

            stack = IoGetCurrentIrpStackLocation(nextIrp);

            /* Fill out the write transfer */
            rwengine->currentWriteTransfer.buffer = (PUCHAR)nextIrp->AssociatedIrp.SystemBuffer;
            rwengine->currentWriteTransfer.transferLength = stack->Parameters.Write.Length;
            rwengine->currentWriteTransfer.bytesTransferred = 0;

            /* Attempt to write the maximum amount of bytes into the buffer */
            bytesWritten = rwengine->WriteToBuffer(
                rwengine->userContext,
                rwengine->currentWriteTransfer.buffer,
                rwengine->currentWriteTransfer.transferLength);

            /* Adjust the transfer object */
            rwengine->currentWriteTransfer.buffer += bytesWritten;
            rwengine->currentWriteTransfer.bytesTransferred += bytesWritten;


            if (rwengine->currentWriteTransfer.bytesTransferred == rwengine->currentWriteTransfer.transferLength) {

                /* Complete */
                nextIrp->IoStatus.Status = STATUS_SUCCESS;
                nextIrp->IoStatus.Information = rwengine->currentWriteTransfer.transferLength;
                IoCompleteRequest(nextIrp, IO_SERIAL_INCREMENT);

            } else {

                /* Pend */
                IoCsqInsertIrp(&rwengine->pendingWriteQueue.csq, nextIrp, NULL);

                /* If needed, start the timeout timer */
                if (rwengine->writeTimeoutStrategy == RWENGINE_TIMEOUT_TIME) {
                    RwEngine_StartWriteTimeoutTimer(rwengine);
                }

                return;
            }
        }

    } while (nextIrp != NULL);


    /* If we get here, all the queued requests were completed */
    rwengine->writesArePending = FALSE;
}


/*----------------------------------------------------------------------------*/
static VOID RwEngine_StartWriteTimeoutTimer(PRWENGINE rwengine)
{
    LARGE_INTEGER timeout;

    /* Calculate the timeout */
    timeout.QuadPart =
        ((ULONGLONG)rwengine->writeTimeout.ConstantTimeoutMs +
        ((ULONGLONG)rwengine->writeTimeout.MultiplierTimeoutMs *
        (ULONGLONG)(rwengine->currentWriteTransfer.transferLength - rwengine->currentWriteTransfer.bytesTransferred))) 
            * -10000;
    
    /* Set the timer */
    KeSetTimer(&rwengine->writeTimeoutTimer, timeout, &rwengine->writeTimeoutDpc);
}



/*----------------------------------------------------------------------------*/
VOID RwEngine_FlushQueuedAndPendingIrps(PRWENGINE rwengine)
{
    KIRQL oldirql;
    PIRP Irp;


    /* Raise the irql to synchronize with any DPCs */
    KeRaiseIrql(DISPATCH_LEVEL, &oldirql);


    /* Complete parked write transfer */
    Irp = IoCsqRemoveNextIrp(&rwengine->pendingWriteQueue.csq, NULL);
    if (Irp != NULL) {

        /* Cancel the timeout timer */
        KeCancelTimer(&rwengine->writeTimeoutTimer);

        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = rwengine->currentWriteTransfer.bytesTransferred;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        /* Clear out the transfer object for debugging purposes */
        rwengine->currentWriteTransfer.buffer = NULL;
        rwengine->currentWriteTransfer.transferLength = 0;
        rwengine->currentWriteTransfer.bytesTransferred = 0;
    }


    /* Flush the write queue */
    Irp = IoCsqRemoveNextIrp(&rwengine->writeQueue.csq, NULL);
    while (Irp != NULL) {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        Irp = IoCsqRemoveNextIrp(&rwengine->writeQueue.csq, NULL);
    }


    /* Indicate that no writes are pending */
    rwengine->writesArePending = FALSE;



    /* Complete parked read transfer */
    Irp = IoCsqRemoveNextIrp(&rwengine->pendingReadQueue.csq, NULL);
    if (Irp != NULL) {

        /* Cancel the timeout timer */
        KeCancelTimer(&rwengine->readTimeoutTimer);

        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = rwengine->currentReadTransfer.bytesTransferred;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        /* Clear out the transfer object for debugging purposes */
        rwengine->currentReadTransfer.buffer = NULL;
        rwengine->currentReadTransfer.transferLength = 0;
        rwengine->currentReadTransfer.bytesTransferred = 0;
    }


    /* Flush the read queue */
    Irp = IoCsqRemoveNextIrp(&rwengine->readQueue.csq, NULL);
    while (Irp != NULL) {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        Irp = IoCsqRemoveNextIrp(&rwengine->readQueue.csq, NULL);
    }


    /* Lower the irql to previous level */
    KeLowerIrql(oldirql);
}
