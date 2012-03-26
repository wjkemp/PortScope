/*  csqimpl.c - Cancel-Safe Queue Implementation
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
#include "csqimpl.h"


/*---------------------------------------------------------------------------
    Defines
 ----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
    Variables
 ----------------------------------------------------------------------------*/ 
 
/*---------------------------------------------------------------------------
    Functions
 ----------------------------------------------------------------------------*/
static VOID CsqInsertIrp (IN PIO_CSQ Csq, IN PIRP Irp);
static VOID CsqRemoveIrp(IN PIO_CSQ Csq, IN PIRP Irp);
static PIRP CsqPeekNextIrp(IN PIO_CSQ Csq,IN PIRP Irp, IN PVOID PeekContext);
static VOID CsqAcquireLock(IN PIO_CSQ Csq, OUT PKIRQL Irql);
static VOID CsqReleaseLock(IN PIO_CSQ Csq, IN KIRQL Irql);
static VOID CsqCompleteCanceledIrp(IN PIO_CSQ pCsq, IN PIRP Irp);


/*---------------------------------------------------------------------------*/
VOID CsqInitialize(PCSQ csq, CsqCancelCallback callback, PVOID context)
{
    /* Initialize the Spin Lock */
    KeInitializeSpinLock(&csq->lock);

    /* Initialize the queue */
    InitializeListHead(&csq->queue);

    /* Initialize the csq */
    IoCsqInitialize( &csq->csq,
                     CsqInsertIrp,
                     CsqRemoveIrp,
                     CsqPeekNextIrp,
                     CsqAcquireLock,
                     CsqReleaseLock,
                     CsqCompleteCanceledIrp);

    /* Store the callback parameters */
    csq->callback = callback;
    csq->callbackContext = context;
}


/*---------------------------------------------------------------------------*/
static VOID CsqInsertIrp(IN PIO_CSQ Csq, IN PIRP Irp)
{
    PCSQ csq;

    csq = CONTAINING_RECORD(Csq, CSQ, csq);
    InsertTailList(&csq->queue, &Irp->Tail.Overlay.ListEntry);
}


/*---------------------------------------------------------------------------*/
static VOID CsqRemoveIrp(IN PIO_CSQ Csq, IN PIRP Irp)
{
    PCSQ csq;

    csq = CONTAINING_RECORD(Csq, CSQ, csq);    
    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);
}


/*---------------------------------------------------------------------------*/
static PIRP CsqPeekNextIrp(IN PIO_CSQ Csq, IN PIRP Irp, IN PVOID PeekContext)
{
    PCSQ csq;
    PIRP                   nextIrp = NULL;
    PLIST_ENTRY            nextEntry;
    PLIST_ENTRY            listHead;
    PIO_STACK_LOCATION     irpStack;

    csq = CONTAINING_RECORD(Csq, CSQ, csq);    
    
    listHead = &csq->queue;

    // 
    // If the IRP is NULL, we will start peeking from the listhead, else
    // we will start from that IRP onwards. This is done under the
    // assumption that new IRPs are always inserted at the tail.
    //
        
    if(Irp == NULL) {       
        nextEntry = listHead->Flink;
    } else {
        nextEntry = Irp->Tail.Overlay.ListEntry.Flink;
    }
    
    while(nextEntry != listHead) {
        
        nextIrp = CONTAINING_RECORD(nextEntry, IRP, Tail.Overlay.ListEntry);

        irpStack = IoGetCurrentIrpStackLocation(nextIrp);
        
        //
        // If context is present, continue until you find a matching one.
        // Else you break out as you got next one.
        //
        
        if(PeekContext) {
            if(irpStack->FileObject == (PFILE_OBJECT) PeekContext) {       
                break;
            }
        } else {
            break;
        }
        nextIrp = NULL;
        nextEntry = nextEntry->Flink;
    }

    return nextIrp;
    
}


/*---------------------------------------------------------------------------*/
static VOID CsqAcquireLock(IN PIO_CSQ Csq, OUT PKIRQL Irql)
{
    PCSQ csq;

    csq = CONTAINING_RECORD(Csq, CSQ, csq);    
    KeAcquireSpinLock(&csq->lock, Irql);
}


/*---------------------------------------------------------------------------*/
static VOID CsqReleaseLock(IN PIO_CSQ Csq, IN KIRQL Irql)
{
    PCSQ csq;

    csq = CONTAINING_RECORD(Csq, CSQ, csq);        
    KeReleaseSpinLock(&csq->lock, Irql);
}


/*---------------------------------------------------------------------------*/
static VOID CsqCompleteCanceledIrp(IN PIO_CSQ pCsq, IN PIRP Irp)
{
    PCSQ csq;

    csq = CONTAINING_RECORD(pCsq, CSQ, csq);


    // If a callback was given, call it
    if (csq->callback) {
        csq->callback(csq->callbackContext, Irp);
    } else {

        // Complete the request
        Irp->IoStatus.Status = STATUS_CANCELLED;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }
}
