/*  pscommon.h - Common Defines.
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
#ifndef __PSCOMMON_H__
#define __PSCOMMON_H__

#include <wdm.h>


/*-----------------------------------------------------------------------------
    Module Definitions
 -----------------------------------------------------------------------------*/

#define DBG0(_x_) /* DbgPrint(_x_) */
#define DBG1(_x_) DbgPrint(_x_)
#define DBG2(_x_) DbgPrint(_x_)
#define DBG3(_x_) DbgPrint(_x_)


/*
#define DBG0(_x_)
#define DBG1(_x_)
#define DBG2(_x_)
#define DBG3(_x_)
*/


typedef enum _DEVICE_TYPE {

    DEVICE_TYPE_INVALID = 0,
    DEVICE_TYPE_CONTROL,
    DEVICE_TYPE_FILTER,

} DEVICE_TYPE;



typedef struct
{
  ULONG Type;
  
} COMMON_DEVICE_DATA, *PCOMMON_DEVICE_DATA;



typedef enum _DEVICE_PNP_STATE {

    NotStarted = 0,         // Not started yet
    Started,                // Device has received the START_DEVICE IRP
    StopPending,            // Device has received the QUERY_STOP IRP
    Stopped,                // Device has received the STOP_DEVICE IRP
    RemovePending,          // Device has received the QUERY_REMOVE IRP
    SurpriseRemovePending,  // Device has received the SURPRISE_REMOVE IRP
    Deleted                 // Device has received the REMOVE_DEVICE IRP

} DEVICE_PNP_STATE;



#define INITIALIZE_PNP_STATE(_Data_)    \
        (_Data_)->DevicePnPState =  NotStarted;\
        (_Data_)->PreviousPnPState = NotStarted;

#define SET_NEW_PNP_STATE(_Data_, _state_) \
        (_Data_)->PreviousPnPState =  (_Data_)->DevicePnPState;\
        (_Data_)->DevicePnPState = (_state_);

#define RESTORE_PREVIOUS_PNP_STATE(_Data_)   \
        (_Data_)->DevicePnPState =   (_Data_)->PreviousPnPState;\
        
        
#endif
