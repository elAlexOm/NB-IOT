
#include <stdint.h>
#include "em_usart.h"
#include "em_cmu.h"
#include "em_leuart.h"
#include "em_dma.h"
#include "em_rtc.h"
#include "uart.h"
#include "buffer.h"
#include "events.h"
#include "timer.h"
#include "setting.h"
#include "InitDevice.h"
#include "em_device.h"
#include "em_chip.h"

#define DMA_CHANNEL    									0
#define BUF_MAX        									256

uint8_t rxbuf[BUF_MAX];
DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));

uint8_t rx_buffer_data[UART_RX_BUFFER_LENGTH];
uint8_t tx_buffer_data[UART_TX_BUFFER_LENGTH];

buffer_t rx_buffer, tx_buffer = { 0 };

void leuart_dma_callback( unsigned int channel, bool primary, void *user );

DMA_CB_TypeDef dma_cb = { .cbFunc = leuart_dma_callback, .primary = 0, .userPtr = NULL };

void leuart_dma_callback( unsigned int channel, bool primary, void *user ) {
	event_set( evLEUART_TX_END );
}

void initLeuart(void) {
	LEUART_Init_TypeDef leuart0Init = {
		.enable   			= leuartEnable,       	/* Activate data reception on LEUn_TX pin. */
		.refFreq  			= 0,                    /* Inherit the clock frequency from the LEUART clock source */
		.baudrate 			= 9600,                 /* Baudrate = 9600 bps */
		.databits 			= leuartDatabits8,      /* Each LEUART frame containes 8 databits */
		.parity   			= leuartNoParity,       /* No parity bits in use */
		.stopbits 			= leuartStopbits2,      /* Setting the number of stop bits in a frame to 2 bit periods */
	};

	DMA_Init_TypeDef dmaInit = {
		.hprot        = 0,                  /* No descriptor protection */
		.controlBlock = dmaControlBlock,    /* DMA control block aligned to 256 */
	};

	DMA_CfgChannel_TypeDef chnlCfg = {
		.highPri   				= false,                     /* Normal priority */
		.enableInt 				= false,                     /* No interrupt enabled for callback functions */
		.select    				= DMAREQ_LEUART0_RXDATAV,    /* Set LEUART0 RX data avalible as source of DMA signals */
		.cb        				= &dma_cb,                   /* No callback function */
	};

	DMA_CfgDescr_TypeDef descrCfg = {
	.dstInc  = dmaDataInc1,       /* Increment destination address by one byte */
	.srcInc  = dmaDataIncNone,    /* Do no increment source address  */
	.size    = dmaDataSize1,      /* Data size is one byte */
	.arbRate = dmaArbitrate1,     /* Rearbitrate for each byte recieved */
	.hprot   = 0,                 /* No read/write source protection */
	};

  CMU_ClockSelectSet( cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockSelectSet( cmuClock_LFB, cmuSelect_LFXO);

  CMU_ClockEnable( cmuClock_LEUART0, true);

  LEUART_Reset( LEUART0 );
  LEUART_Init( LEUART0, &leuart0Init );

  LEUART0->SIGFRAME = '\r';
  LEUART_IntEnable( LEUART0, LEUART_IEN_SIGF | LEUART_IEN_RXDATAV );

  NVIC_EnableIRQ(LEUART0_IRQn);

  LEUART0->ROUTE = LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | LEUART_ROUTE_LOCATION_LOC1;

  DMA_Init(&dmaInit);
  DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);
  DMA_CfgDescr(DMA_CHANNEL, true, &descrCfg);

  DMA_ActivateBasic( DMA_CHANNEL, true, false, (void *) &rxbuf, (void *) &LEUART0->RXDATA, BUF_MAX-1 );
}

void init_uart( void ) {

	initLeuart();

	buffer_init( &rx_buffer, rx_buffer_data, sizeof( rx_buffer_data ));
	buffer_init( &tx_buffer, tx_buffer_data, sizeof( tx_buffer_data ));

	NVIC_EnableIRQ( LEUART0_IRQn );

	NVIC_EnableIRQ( USART1_RX_IRQn );
	NVIC_EnableIRQ( USART1_TX_IRQn );

	USART1->IEN |= ( 1 << 2 );
	USART1->IEN |= ( 1 << 2 );
}

/** \brief Отправка сообщения по Leuart
 *
 */
void leuart_send( char* mess, uint8_t length ) {

	dmaControlBlock->SRCEND = mess + length;

  LEUART0->CTRL = LEUART_CTRL_TXDMAWU;

  DMA_ActivateBasic(DMA_CHANNEL,                  /* Activate channel selected */
                    true,                         /* Use primary descriptor */
                    false,                        /* No DMA burst */
                    NULL,                         /* Keep destination address */
                    NULL,                         /* Keep source address*/
                    BUF_MAX - 1);                 /* Size of buffer minus1 */
}

/** \brief Отправка сообщения по Uart
 *
 */
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

void LEUART0_IRQHandler(void) {
  uint32_t leuartif = LEUART_IntGet(LEUART0);
  LEUART_IntClear(LEUART0, leuartif);

  if (leuartif & LEUART_IF_SIGF)
  {
//    DMA_ActivateBasic(DMA_CHANNEL, true, false, NULL, NULL, BUF_MAX-1);

//		BSP_LedToggle(1);
//		for (j = 0; j < 1000; j++);
//		BSP_LedToggle(1);
  }
  if( leuartif & LEUART_IF_RXDATAV ) {
  }
}
