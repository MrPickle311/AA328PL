/*
 * static_queue.c
 *
 * Created: 03/04/2021 19:37:56
 *  Author: Damian Wójcik
 */ 

#include "circular_buffer.h"
#include <util/atomic.h>
#include <assert.h>

struct CircularBuffer_t
{
	byte_t* body_;
	length_t head_;
	length_t tail_;
	length_t capacity_;
	bool is_full_;
};

CircularBuffer CircularBuffer_initNormal(length_t size)
{
	CircularBuffer buffer_temp;

#ifdef DEBUG
		assert(size != 0);
#endif
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		buffer_temp = malloc(sizeof(CircularBuffer_t));
	
#ifdef DEBUG
		assert(buffer_temp != NULL);
#endif

		buffer_temp->body_ = malloc(sizeof(byte_t)*size);
	}
	
	buffer_temp->capacity_ = size;
	CircularBuffer_clear(buffer_temp);
		
#ifdef DEBUG
	assert(CircularBuffer_isEmpty(buffer_temp));
#endif

	return buffer_temp;
}

CircularBuffer CircularBuffer_initWithArrayProvided(byte_t* buffer,length_t size)
{
	CircularBuffer buffer_temp;
		
#ifdef DEBUG
		assert(buffer != NULL && size != 0);
#endif
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		buffer_temp = malloc(sizeof(CircularBuffer_t));

#ifdef DEBUG
		assert(buffer_temp != NULL);
#endif

	}
		
	buffer_temp->body_ = buffer;
	buffer_temp->capacity_ = size;
	CircularBuffer_clear(buffer_temp);
		
#ifdef DEBUG
		assert(CircularBuffer_isEmpty(buffer_temp));
#endif
	
	return buffer_temp;
}


void CircularBuffer_SoftRelease(CircularBuffer buffer)
{
	
#ifdef DEBUG
		assert(buffer != NULL);
#endif
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		free(buffer);
	}
}

void CircularBuffer_HardRelease(CircularBuffer buffer)
{
	
#ifdef DEBUG
		assert(buffer != NULL);
		assert(buffer->body_ != NULL);
#endif
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		free(buffer->body_);
		free(buffer);
	}
}

void CircularBuffer_clear(CircularBuffer buffer)
{

#ifdef DEBUG
	assert(buffer);
#endif

	buffer->head_ = 0;
	buffer->tail_ = 0;
	buffer->is_full_ = false;
}

//extracted pointer advancment logic 
static inline void _advancePtr(CircularBuffer buffer)
{
	
#ifdef DEBUG
		assert(buffer);
#endif

	if(buffer->is_full_)
		if(++buffer->tail_ == buffer->capacity_)
			buffer->tail_ = 0;
		
	if(++buffer->head_ == buffer->capacity_)
		buffer->head_ = 0;
		
	buffer->is_full_ = buffer->head_ == buffer->tail_;
	
}

//this helper function is called when when removing a value from the buffer
static inline void _retreatPtr(CircularBuffer buffer)
{
	
#ifdef DEBUG
		assert(buffer);
#endif
	buffer->is_full_ = false;
	
	if(++buffer->tail_ == buffer->capacity_)
		buffer->tail_ = 0;
}

enum OperationStatus CircularBuffer_safePush(CircularBuffer buffer,byte_t data)
{
	enum OperationStatus status = Failure;
	
#ifdef DEBUG
		assert(buffer != 0 && buffer->body_ != NULL);
#endif

		if(!buffer->is_full_)
		{
			buffer->body_[buffer->head_] = data;
			_advancePtr(buffer);
			status = Success;
		}
	
	return status;
}


void CircularBuffer_forcePush(CircularBuffer buffer,byte_t data)
{
	
#ifdef DEBUG
		assert(buffer != 0 && buffer->body_ != NULL);
#endif
	buffer->body_[buffer->head_] = data;
	
	_advancePtr(buffer);
	
}

enum OperationStatus CircularBuffer_pop(CircularBuffer buffer,byte_t* dest)
{
#ifdef DEBUG
	assert(buffer != 0 && buffer->body_ != NULL);
#endif

	enum OperationStatus status = Failure;
	
	if (!CircularBuffer_isEmpty(buffer))
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			*dest = buffer->body_[buffer->tail_];
		}
		_retreatPtr(buffer);
		status = Success;
		
	}
	
	return status;
}

bool CircularBuffer_isEmpty(CircularBuffer buffer)
{

#ifdef DEBUG
	assert(buffer);
#endif

	return !buffer->is_full_ && buffer->head_ == buffer->tail_ ;

}

bool CircularBuffer_isFull(CircularBuffer buffer)
{

#ifdef DEBUG
	assert(buffer);
#endif	
	
	return buffer->is_full_;
}

length_t CircularBuffer_getCapacity(CircularBuffer buffer)
{
#ifdef DEBUG
	assert(buffer);
#endif

	return buffer->capacity_;
}

static inline void xd(){}

length_t CircularBuffer_getFillLevel(CircularBuffer buffer)
{
	
#ifdef DEBUG
		assert(buffer);
#endif
	length_t size = buffer->capacity_;
	
	if(!buffer->is_full_)
	{
		if(buffer->head_ >= buffer->tail_)
			size = buffer->head_ - buffer->tail_;
		else size = buffer->capacity_ + buffer->head_ - buffer->tail_;
	}
	return size;
}
