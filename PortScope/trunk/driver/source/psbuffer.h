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
	unsigned char* buffer;
	ULONG length;
	ULONG readPtr;
	ULONG writePtr;
	ULONG count;
	
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
void Buffer_Put(BUFFER* obj, unsigned char value);


/**
 * @brief Extracts a byte from the buffer
 * @param obj The buffer object
 * @return The byte that was extracted
 */
unsigned char Buffer_Get(BUFFER* obj);


#endif
