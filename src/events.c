
/** \file 		events.c
 *  \author		Alex
 *  \date		11.07.2017
 */

#include "events.h"
#include <stdint.h>
#include "buffer.h"

uint8_t event_buffer_data[EVENT_COUNT];

buffer_t event_buffer = { 0, 0, 0, sizeof( event_buffer_data ), event_buffer_data };

void event_set( uint8_t event ) {
	buffer_set( &event_buffer, event );
}

uint8_t event_get( void ) {
	return( buffer_get( &event_buffer ));
}

uint8_t event_count( void ) {
	return( buffer_count( &event_buffer ));
}
