
/** \file 		uart.h
 * 	\author		Alex
 * 	\date		11.07.2017
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define UART_BUFFER_LENGTH			64

void init_uart( void );
int16_t uart_get( void );
bool uart_send( uint8_t* data, uint8_t count );
uint8_t uart_tx_buffer_free_space( void );
uint8_t uart_rx_buffer_fill_space( void );
void uart_clear_rx_buffer( void );
void uart_clear_tx_buffer( void );

void leuart_send( char* mess, uint8_t length );
