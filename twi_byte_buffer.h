#ifndef TWI_BUFFER_H_
#define TWI_BUFFER_H_

#include "global_utils.h"

struct TWI_NormalBuffer_Status
{
	union
	{
		struct
		{
			//flags 
			byte_t data_ready_to_process_			: 1;
			byte_t data_is_received_				: 1;
			byte_t data_is_transmitted_				: 1;
			//current count of bytes in a buffer
			length_t in_buffer_current_bytes_count	: 5;//max buffer index = 2^5 - 1
		};
		length_t as_word;//for clearing 
	};
};

typedef struct TWI_NormalBuffer_Status TWI_NormalBuffer_Status;

struct TWI_LargeBuffer_Status
{
	union
	{
		struct
		{
			//flags
			byte_t data_ready_to_process_					: 1;
			byte_t data_is_received_						: 1;
			byte_t data_is_transmitted_						: 1;
			//current count of bytes in a buffer
			large_length_t in_buffer_current_bytes_count	: 13;//max buffer index = 2^13 - 1
		};
		double_byte_t as_word;//for clearing
	};
};

typedef struct TWI_LargeBuffer_Status TWI_LargeBuffer_Status;

struct TWI_NormalBuffer
{
	TWI_NormalBuffer_Status buffer_status_;
	byte_t* body_;
	length_t capacity_;
};

typedef struct TWI_NormalBuffer TWI_NormalBuffer;

struct TWI_LargeBuffer
{
	TWI_LargeBuffer_Status buffer_status_;
	byte_t* body_;
	large_length_t capacity_;
};

typedef struct TWI_LargeBuffer TWI_LargeBuffer;

//allocating and initialization
//KOMMAND ZMIEN NA DATA
void TWI_NormalBuffer_Init(volatile TWI_NormalBuffer* buffer,length_t size);

void TWI_LargeBuffer_Init(volatile TWI_LargeBuffer* buffer,large_length_t size);

//reseting macros

#define TWI_Buffer_TotalReset(buffer)								buffer.buffer_status_.as_word = 0

#define TWI_Buffer_ResetBytes(buffer)								buffer.buffer_status_.in_buffer_current_bytes_count = 0

//setting states macros

#define TWI_Buffer_Set_DataIsReceived_Status(buffer)				buffer.buffer_status_.data_is_received_		  = 1	
																													 
#define TWI_Buffer_Set_DataIsTransmitted_Status(buffer)				buffer.buffer_status_.data_is_transmitted_    = 1
																														 
#define TWI_Buffer_Set_DataIsReadyToProcess_Status(buffer)			buffer.buffer_status_.data_ready_to_process_  = 1

//clearing states macros

#define TWI_Buffer_Clear_DataIsReceived_Status(buffer)				buffer.buffer_status_.data_is_received_		  = 0
																												  
#define TWI_Buffer_Clear_DataIsTransmitted_Status(buffer)			buffer.buffer_status_.data_is_transmitted_	  = 0

#define TWI_Buffer_Clear_DataIsReadyToProcess_Status(buffer)		buffer.buffer_status_.data_ready_to_process_  = 0

//checking states macros

#define TWI_Buffer_Is_DataReceived(buffer)							( buffer.buffer_status_.data_is_received_		     == 1 )
																													   		 
#define TWI_Buffer_Is_DataTransmitted(buffer)						( buffer.buffer_status_.data_is_transmitted_	   	 == 1 )
																													   
#define TWI_Buffer_Is_DataIsReadyToProcess(buffer)					( buffer.buffer_status_.data_ready_to_process_       == 1 )

//indexing

#define TWI_Buffer_AtCurrentByte(buffer)							buffer.body_[buffer.buffer_status_.in_buffer_current_bytes_count]

#define TWI_Buffer_AtByte(buffer,pos)								buffer.body_[pos]

#define TWI_Buffer_GoNextByte(buffer)								++buffer.buffer_status_.in_buffer_current_bytes_count					

#define TWI_Buffer_GoPreviousByte(buffer)							--buffer.buffer_status_.in_buffer_current_bytes_count

//overflow and indexing control

#define TWI_Buffer_GetCurrentPos(buffer)						    buffer.buffer_status_.in_buffer_current_bytes_count							

#define TWI_Buffer_OverflowOccured(buffer)						    ( buffer.buffer_status_.in_buffer_current_bytes_count >= buffer.capacity_ )

#define TWI_Buffer_IsAtFirstByte(buffer)						    ( buffer.buffer_status_.in_buffer_current_bytes_count == 0 )

#endif /* TWI_BUFFER_H_ */