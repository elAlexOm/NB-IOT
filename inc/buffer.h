
#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint16_t in;
	uint16_t out;
	uint16_t count;
	uint16_t size;
	uint8_t* buffer;
} buffer_t;

bool buffer_init( buffer_t* buff, uint8_t* data, uint16_t size );
uint16_t buffer_set( buffer_t* buff, uint8_t data );
int16_t buffer_get( buffer_t* buff );
uint16_t buffer_count( buffer_t* buff );
uint16_t buffer_free( buffer_t* buff );
void buffer_clear( buffer_t* buff );
