/*  psbuffer.c - Circular Buffer
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
#include <wdm.h>
#include "psbuffer.h"


/*---------------------------------------------------------------------------
    Defines
 ----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
    Variables
 ----------------------------------------------------------------------------*/ 
 
/*---------------------------------------------------------------------------
    Functions
 ----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
void Buffer_Initialize(BUFFER* obj, unsigned char* buffer, ULONG length)
{
	obj->head = buffer;
    obj->tail = (buffer + length);
	obj->read = obj->head;
	obj->write = obj->head;
	obj->length = length;
	obj->count = 0;

    KeInitializeSpinLock(&obj->lock);

}


/*---------------------------------------------------------------------------*/
void Buffer_Clear(BUFFER* obj)
{
	obj->read = obj->head;
	obj->write = obj->head;
	obj->count = 0;	
}


/*---------------------------------------------------------------------------*/
ULONG Buffer_Put(BUFFER* obj, const PUCHAR data, ULONG length)
{
    ULONG headCount;
    KIRQL irql;


    KeAcquireSpinLock(&obj->lock, &irql);

    /* Limit the length to the available size */
    if (length > (obj->length - obj->count)) {
        length = (obj->length - obj->count);
    }

    if (length) {
        headCount = (ULONG)(obj->tail - obj->write);

        /* Wrap around */
        if (headCount < length) {
            ULONG tailCount;

            tailCount = (length - headCount);
            RtlCopyMemory(obj->write, data, headCount);
            RtlCopyMemory(obj->head, (data + headCount), tailCount);
            obj->write = (obj->head + tailCount);

        /* Enough to hold entire write */
        } else if (headCount > length) {
            RtlCopyMemory(obj->write, data, length);
            obj->write += length;

        /* Wrap around with no tail */
        } else {
            RtlCopyMemory(obj->write, data, length);
            obj->write = obj->head;
        }

        obj->count += length;
    }

    KeReleaseSpinLock(&obj->lock, irql);

    return length;
}


/*---------------------------------------------------------------------------*/
ULONG Buffer_Get(BUFFER* obj, PUCHAR data, ULONG length)
{
    ULONG headCount;
    KIRQL irql;


    KeAcquireSpinLock(&obj->lock, &irql);

    /* Limit the length to the available data */
    if (length > obj->count) {
        length = obj->count;
    }

    if (length) {
        headCount = (ULONG)(obj->tail - obj->read);

        /* Wrap around */
        if (headCount < length) {
            ULONG tailCount;

            tailCount = (length - headCount);
            RtlCopyMemory(data, obj->read, headCount);
            RtlCopyMemory((data + headCount), obj->head, tailCount);
            obj->read = (obj->head + tailCount);

        /* Enough to hold entire write */
        } else if (headCount > length) {
            RtlCopyMemory(data, obj->read, length);
            obj->read += length;

        /* Wrap around with no tail */
        } else {
            RtlCopyMemory(data, obj->read, length);
            obj->read = obj->head;
        }

        obj->count -= length;
    }

    KeReleaseSpinLock(&obj->lock, irql);

    return length;
}
