/*
 * 	\file			at_command.c
 *	\author			Alex
 * 	\date 			12 июля 2017 г.2017
 */

#include <stdint.h>
#include <string.h>

#include "uart.h"
#include "at_command.h"
#include <string.h>
#include <stdlib.h>
#include "setting.h"

const char* at_responce[] = {
	"OK",
	"ERROR",
	"Neul",
	"+CEREG:",
	"+NPING:",
	"+NSONMI:",
	"+NNMI:",
	"+NSMI:"
};

#define AT_ITEM_COUNT			( sizeof( at_responce ) / sizeof( char* ))

uint8_t at_rx_buffer[AT_RX_BUFFER_LENGTH];
uint8_t at_rx_index = 0;

at_status_t at_parse_mess( char* mess ) {
	uint8_t i;

	volatile static uint8_t counter = 0;

	for( i = 0; i < AT_ITEM_COUNT; i++ ) {
		if( strstr( mess, at_responce[i] )) break;
	}
	if( i < AT_ITEM_COUNT ) {
		switch( i ) {
			case 0 : {
				counter++;
				break;
			}
			case 1 : {
				break;
			}
			case 2 : {
				counter--;
				break;
			}
		}
	}
	return AT_OK;
}

at_status_t at_receive( void ) {
	while( 1 ) {
		int16_t sym;

		if( at_rx_index == sizeof( at_rx_buffer )) {
			at_rx_index = 0;
		}

		sym = uart_get();

		if( sym == -1 ) break;

		at_rx_buffer[at_rx_index++] = ( uint8_t )sym;

		if( at_rx_index < 2 ) continue;

		if(( at_rx_buffer[at_rx_index-2] == '\r' ) && ( at_rx_buffer[at_rx_index - 1] == '\n' )) {
			at_rx_buffer[at_rx_index - 2] = 0;
			at_parse_mess(( char* )at_rx_buffer );
			at_rx_index = 0;
#ifdef AT_DEBUG
			memset( at_rx_buffer, 0, sizeof( at_rx_buffer ));
#endif
		}
	}
	return AT_OK;
}

at_status_t at_sent( char* command ) {
	if( uart_send( ( uint8_t* )command, strlen( command ))) return AT_OK;
	return AT_ERROR;
}
