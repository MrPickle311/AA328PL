#include "twi_byte_buffer.h"
#include <util/atomic.h>
#include <stdlib.h>

void TWI_NormalBuffer_Init(volatile TWI_NormalBuffer* buffer,length_t size)
{
	buffer->buffer_status_.as_word = 0;
	buffer->capacity_ = size;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		buffer->body_ = calloc(size,sizeof(byte_t));
	}
}

void TWI_LargeBuffer_Init(volatile TWI_LargeBuffer* buffer,large_length_t size)
{
	buffer->buffer_status_.as_word = 0;
	buffer->capacity_ = size;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		buffer->body_ = calloc(size,sizeof(byte_t));
	}
}
