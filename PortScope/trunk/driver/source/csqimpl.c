/******************************************************************************
    Copyright (c) 2005 Xstream Flow (Pty) Ltd.

    Provides an implementation for cancel-safe IRP queues.
    See the sample that comes with the DDK.
        
 ******************************************************************************/
#include "csqimpl.h"


/*---------------------------------------------------------------------------
 .  Local function definitions
 ----------------------------------------------------------------------------*/
VOID CsqInsertIrp (IN PIO_CSQ Csq, IN PIRP Irp);
VOID CsqRemoveIrp(IN PIO_CSQ Csq, IN PIRP Irp);
PIRP CsqPeekNextIrp(IN PIO_CSQ Csq,IN PIRP Irp, IN PVOID PeekContext);
VOID CsqAcquireLock(IN PIO_CSQ Csq, OUT PKIRQL Irql);
VOID CsqReleaseLock(IN PIO_CSQ Csq, IN KIRQL Irql);
VOID CsqCompleteCanceledIrp(IN PIO_CSQ pCsq, IN PIRP Irp);

/*---------------------------------------------------------------------------
 .  Module Definitions
 ----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 .  Module Variables
 ----------------------------------------------------------------------------*/ 
 

 /*---------------------------------------------------------------------------
 .  Module Interface Functions
 ----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 . CsqInitialize
 ----------------------------------------------------------------------------*/
VOID CsqInitialize(PCSQ csq, CsqCancelCallback callback, PVOID context) {


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


/*-----------------------------------------------------------------------------
 . CsqInsertIrp
 ----------------------------------------------------------------------------*/
VOID CsqInsertIrp(IN PIO_CSQ Csq, IN PIRP Irp) {

    PCSQ csq;

    csq = CONTAINING_RECORD(Csq, CSQ, csq);
    InsertTailList(&csq->queue, &Irp->Tail.Overlay.ListEntry);
}


/*-----------------------------------------------------------------------------
 . CsqRemoveIrp
 ----------------------------------------------------------------------------*/
VOID CsqRemoveIrp(IN PIO_CSQ Csq, IN PIRP Irp) {
    PCSQ csq;

    csq = CONTAINING_RECORD(Csq, CSQ, csq);    
    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);
}


/*-----------------------------------------------------------------------------
 . CsqPeekNextIrp
 ----------------------------------------------------------------------------*/
PIRP CsqPeekNextIrp(IN PIO_CSQ Csq, IN PIRP Irp, IN PVOID PeekContext) {
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


/*-----------------------------------------------------------------------------
 . CsqAcquireLock
 ----------------------------------------------------------------------------*/
VOID CsqAcquireLock(IN PIO_CSQ Csq, OUT PKIRQL Irql) {
    PCSQ csq;

    csq = CONTAINING_RECORD(Csq, CSQ, csq);    
    KeAcquireSpinLock(&csq->lock, Irql);
}


/*-----------------------------------------------------------------------------
 . CsqReleaseLock
 ----------------------------------------------------------------------------*/
VOID CsqReleaseLock(IN PIO_CSQ Csq, IN KIRQL Irql) {
    PCSQ csq;

    csq = CONTAINING_RECORD(Csq, CSQ, csq);        
    KeReleaseSpinLock(&csq->lock, Irql);
}


/*-----------------------------------------------------------------------------
 . CsqCompleteCanceledIrp
 ----------------------------------------------------------------------------*/
VOID CsqCompleteCanceledIrp(IN PIO_CSQ pCsq, IN PIRP Irp) {

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



