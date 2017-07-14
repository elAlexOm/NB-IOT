
#include <stdint.h>
#include "em_usart.h"
#include "em_cmu.h"
#include "em_leuart.h"

#include "uart.h"
#include "buffer.h"

#include "events.h"

#include "timer.h"
#include "setting.h"

uint8_t rx_buffer_data[UART_RX_BUFFER_LENGTH];
uint8_t tx_buffer_data[UART_TX_BUFFER_LENGTH];

buffer_t rx_buffer, tx_buffer = { 0 };

void init_uart( void ) {
	LEUART_Init_TypeDef leuart1Init = {
	  .enable   = leuartEnable,       	/* Activate data reception on LEUn_TX pin. */
	  .refFreq  = 0,                    /* Inherit the clock frequenzy from the LEUART clock source */
	  .baudrate = 9600,                 /* Baudrate = 9600 bps */
	  .databits = leuartDatabits8,      /* Each LEUART frame containes 8 databits */
	  .parity   = leuartNoParity,       /* No parity bits in use */
	  .stopbits = leuartStopbits2,      /* Setting the number of stop bits in a frame to 2 bitperiods */
	};

	LEUART_Reset( LEUART0 );
	LEUART_Init( LEUART0, &leuart1Init );

	/* Route LEUART1 TX pin to DMA location 0 */
	LEUART0->ROUTE = LEUART_ROUTE_TXPEN | LEUART_ROUTE_LOCATION_LOC0 | LEUART_ROUTE_RXPEN;


	buffer_init( &rx_buffer, rx_buffer_data, sizeof( rx_buffer_data ));
	buffer_init( &tx_buffer, tx_buffer_data, sizeof( tx_buffer_data ));

	NVIC_EnableIRQ( LEUART0_IRQn );

	NVIC_EnableIRQ( USART1_RX_IRQn );
	NVIC_EnableIRQ( USART1_TX_IRQn );

	USART1->IEN |= ( 1 << 2 );
	USART1->IEN |= ( 1 << 2 );
}

bool uart_send( uint8_t* data, uint8_t count ) {
	if( count > buffer_free( &tx_buffer )) return false;
	for( uint8_t index = 0; index < count; index++ ) {
		buffer_set( &tx_buffer, data[index] );
	}
	USART1->IEN |= ( 1 << 1 );
	return true;
}

int16_t uart_get( void ) {
	return buffer_get( &rx_buffer );
}

void uart_clear_rx_buffer( void ) {
	buffer_clear( &rx_buffer );
}

void uart_clear_tx_buffer( void ) {
	buffer_clear( &tx_buffer );
}

uint8_t uart_tx_buffer_free_space( void ) {
	return buffer_free( &tx_buffer );
}

uint8_t uart_rx_buffer_fill_space( void ) {
	return buffer_count( &tx_buffer );
}

void USART1_RX_IRQHandler( void ) {
	if ( USART1->IF & USART_IF_RXDATAV ) {
		buffer_set( &rx_buffer, USART1->RXDATA );
		event_set( evUART_RX );
		START_TIMER( tmrRX_MESS, AT_RX_MESS_END_DELAY );
	}
}

void USART1_TX_IRQHandler( void ) {
	if( USART1->IF & USART_IF_TXBL ) {
		if( tx_buffer.count > 0) {
			USART1->TXDATA = ( uint8_t )buffer_get( &tx_buffer );
		}
		if ( tx_buffer.count == 0) {
			USART1->IEN &= ~( 1 << 1 );
			event_set( evUART_TX_END );
		}
	}
}

void LEUART0_IRQHandler( void ) {

}
