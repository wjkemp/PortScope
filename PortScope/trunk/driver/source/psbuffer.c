/******************************************************************************
 * buffer.c
 ******************************************************************************/
#include "psbuffer.h"
#include <string.h>


//----------------------------------------------------------------------------
void Buffer_Initialize(BUFFER* obj, unsigned char* buffer, ULONG length)
{
	obj->buffer = buffer;
	obj->length = length;
	obj->readPtr = 0;
	obj->writePtr = 0;
	obj->count = 0;
}


//----------------------------------------------------------------------------
void Buffer_Clear(BUFFER* obj)
{
	obj->readPtr = 0;
	obj->writePtr = 0;
	obj->count = 0;	
}


//----------------------------------------------------------------------------
void Buffer_Put(BUFFER* obj, unsigned char value)
{
	if (obj->count < obj->length) {
		obj->buffer[obj->writePtr] = value;
		obj->count++;
		obj->writePtr++;
		if (obj->writePtr == obj->length) {
			obj->writePtr = 0;
		}
	}	
}


//----------------------------------------------------------------------------
unsigned char Buffer_Get(BUFFER* obj)
{
	unsigned char value = 0;
	
	if (obj->count > 0) {
		value = obj->buffer[obj->readPtr];
		obj->count--;
		obj->readPtr++;
		if (obj->readPtr == obj->length) {
			obj->readPtr = 0;
		}
	}	
	
	return value;	
}
