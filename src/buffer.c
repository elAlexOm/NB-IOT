/**
 * 	\file		buffer.c
 *	\author 	Author: Alex
 *  \date 		10 июля 2017 г.
 *
 */

#include "buffer.h"

bool buffer_init( buffer_t* buff, uint8_t* data, uint16_t size ) {
	buff->in 		= 0;
	buff->out 		= 0;
	buff->count 	= 0;
	buff->buffer 	= data;
	buff->size 		= size;
	return true;
}

uint16_t buffer_set( buffer_t* buff, uint8_t data ) {
	if( buff->count < buff->size ) {
		buff->buffer[buff->in++] = data;
		if( buff->in >= buff->size ) buff->in = 0;
		buff->count++;
	}
	return ( buffer_count( buff ));
}

int16_t buffer_get( buffer_t* buff ) {
	int16_t result;

	if( buff->count == 0 ) return -1;
	result = buff->buffer[buff->out];
	if( ++buff->out >= buff->size ) buff->out = 0;
	buff->count--;
	return result;
}

uint16_t buffer_count( buffer_t* buff ) {
	return buff->count;
}

uint16_t buffer_free( buffer_t* buff ) {
	return ( buff->size - buff->count );
}

void buffer_clear( buffer_t* buff ) {
	for( uint16_t i = 0; i < buff->size; i++ ) {
		buff->buffer[i] = 0;
	}
	buff->in 	= 0;
	buff->out 	= 0;
	buff->count = 0;
}
