/******************************************************************************
    Copyright (c) 2005 Xstream Flow (Pty) Ltd.

    Provides an implementation for cancel-safe IRP queues.
    See the sample that comes with the DDK.
        
 ******************************************************************************/
#ifndef __CSQIMPL_H__
#define __CSQIMPL_H__

#include <wdm.h>
#include <csq.h>


/*-----------------------------------------------------------------------------
 . Module Definitions
 -----------------------------------------------------------------------------*/

typedef VOID (*CsqCancelCallback)(PVOID, PIRP);


typedef struct _CSQ {

    IO_CSQ csq;   
    KSPIN_LOCK lock;
    LIST_ENTRY queue;
    PVOID callbackContext;
    CsqCancelCallback callback;


} CSQ, *PCSQ;


/*-----------------------------------------------------------------------------
 . Module Interface
 -----------------------------------------------------------------------------*/
VOID CsqInitialize(PCSQ csq, CsqCancelCallback callback, PVOID context);

#endif
