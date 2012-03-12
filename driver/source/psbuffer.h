/******************************************************************************/
/** @file buffer.h
 *  @brief Circular Byte Buffer
 */   
/******************************************************************************/
#ifndef __PSBUFFER_H__
#define __PSBUFFER_H__

#include <ntddk.h>


/*------------------------------------------------------------------------------
    Module Definitions
 -------------------------------------------------------------------------------*/


/** Buffer Object */
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


/**
 * @brief Initializes the buffer
 * @param obj The buffer object
 * @param buffer A pointer to a byte array
 * @param length The size of the byte array
 */
void Buffer_Initialize(BUFFER* obj, unsigned char* buffer, ULONG length);

 
/**
 * @brief Clears the buffer
 * @param obj The buffer object
 */
void Buffer_Clear(BUFFER* obj);


/**
 * @brief Inserts a byte into the buffer
 * @param obj The buffer object
 * @param value The byte to insert
 */
ULONG Buffer_Put(BUFFER* obj, const PUCHAR data, ULONG length);


/**
 * @brief Extracts a byte from the buffer
 * @param obj The buffer object
 * @return The byte that was extracted
 */
ULONG Buffer_Get(BUFFER* obj, PUCHAR data, ULONG length);


#endif
