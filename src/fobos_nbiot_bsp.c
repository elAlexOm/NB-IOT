
/** \file 		fobos_nbiot_bsp.с
 * 	\author		Alex
 * 	\date		11.07.2017
 *
 * 	Описание платы Fobos NB-IOT
 */

#include "fobos_nbiot_bsp.h"
#include "em_system.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_assert.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "events.h"
#include "setting.h"
#include "timer.h"

uint8_t old_key	= 0;

void init_board( void ) {
	GPIO_IntConfig( gpioPortD, 4, true, true, true );
	GPIO_IntConfig( gpioPortD, 5, true, true, true );

	NVIC_EnableIRQ( GPIO_EVEN_IRQn );
	NVIC_EnableIRQ( GPIO_ODD_IRQn );
}

void key_event( void ) {
	uint8_t keys = 0;
	uint8_t change;

	if( !GPIO_PinInGet( gpioPortD, 4 )) {
		keys |= ( 1 << 0 );
	}
	if( !GPIO_PinInGet( gpioPortD, 5 )) {
		keys |= ( 1 << 1 );
	}
	if( old_key == keys ) return;

	change = keys ^ old_key;

	if( change & ( 1 << 0 )) {
		if( keys & ( 1 << 0 )) event_set( evKEY1_ON );
		else event_set( evKEY1_OFF );
	}
	if( change & ( 1 << 1 )) {
		if( keys & ( 1 << 1 )) event_set( evKEY2_ON );
		else event_set( evKEY2_OFF );
	}
}

void GPIO_EVEN_IRQHandler( void ) {
	uint32_t flag = GPIO_IntGet();

	switch( flag ) {
		case ( 1 << 4 ) : {
			START_TIMER( tmrKEYS, KEY_DELAY );
/*			if( !GPIO_PinInGet( gpioPortD, 4 )) event_set( evKEY1_ON );
			else event_set( evKEY1_OFF );
			break;*/
		}
	}
	GPIO_IntClear( flag );
}

void GPIO_ODD_IRQHandler( void ) {
	uint32_t flag = GPIO_IntGet();

	switch( flag ) {
		case ( 1 << 5 ) : {
			START_TIMER( tmrKEYS, KEY_DELAY );
/*			if( !GPIO_PinInGet( gpioPortD, 5 )) event_set( evKEY2_ON );
			else event_set( evKEY2_OFF );
			break;*/
		}
	}
	GPIO_IntClear( flag );
}

