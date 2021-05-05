/*
 * static_queue.h
 *
 * Created: 03/04/2021 19:18:54
 *  Author: Damian Wójcik
 */ 


#ifndef STATIC_QUEUE_H_
#define STATIC_QUEUE_H_

#include "global_utils.h"

typedef struct CircularBuffer_t CircularBuffer_t;

typedef CircularBuffer_t* CircularBuffer;

CircularBuffer CircularBuffer_initNormal(length_t size);

CircularBuffer CircularBuffer_initWithArrayProvided(byte_t* buffer,length_t size);

//do not release CircularBuffer body
void CircularBuffer_SoftRelease(CircularBuffer buffer);

//release CircularBuffer body
void CircularBuffer_HardRelease(CircularBuffer buffer);

//empty a CircularBuffer
void CircularBuffer_clear(CircularBuffer buffer);

//will not overwrite data when CircularBuffer is full
enum OperationStatus CircularBuffer_safePush(CircularBuffer buffer,byte_t data);

//this version can overwrite
void CircularBuffer_forcePush(CircularBuffer buffer, byte_t data);

//returns Failure if empty, otherwise Success
enum OperationStatus CircularBuffer_pop(CircularBuffer buffer,byte_t* dest);

bool CircularBuffer_isEmpty(CircularBuffer buffer);

bool CircularBuffer_isFull(CircularBuffer buffer);

//return max size of a buffer
length_t CircularBuffer_getCapacity(CircularBuffer buffer);

//returns a current number of elements in a buffer
length_t CircularBuffer_getFillLevel(CircularBuffer buffer);

#endif /* STATIC_QUEUE_H_ */