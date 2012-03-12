/*  psbuffer.h - Circular Buffer
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
#ifndef __PSBUFFER_H__
#define __PSBUFFER_H__


/*------------------------------------------------------------------------------
    Module Definitions
 -------------------------------------------------------------------------------*/

/* Buffer Object */
typedef struct
{
	PUCHAR head;
    PUCHAR tail;
    PUCHAR read;
    PUCHAR write;

	ULONG count;
	ULONG length;

    KSPIN_LOCK lock;
	
} BUFFER, *PBUFFER;


/*------------------------------------------------------------------------------
    Module Interface
 -------------------------------------------------------------------------------*/
void Buffer_Initialize(BUFFER* obj, unsigned char* buffer, ULONG length);
void Buffer_Clear(BUFFER* obj);
ULONG Buffer_Put(BUFFER* obj, const PUCHAR data, ULONG length);
ULONG Buffer_Get(BUFFER* obj, PUCHAR data, ULONG length);


#endif
