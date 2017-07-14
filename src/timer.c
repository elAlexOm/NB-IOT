/*
 * timer.c
 *
 *  Created on: 12 июля 2017 г.
 *      Author: Алексей
 */

#include "em_cmu.h"
#include "em_timer.h"
#include "em_device.h"
#include "em_chip.h"

#include "InitDevice.h"

#include "timer.h"
#include "events.h"

uint32_t timers[TIMERS_COUNT];

void init_timers( void ) {
	TIMER_Init_TypeDef timer1 = { 0 };

	timer1.mode 	= timerModeUp;
	timer1.clkSel 	= timerClkSelHFPerClk;
	timer1.prescale	= timerPrescale1;
	timer1.enable	= true;

	CMU_ClockEnable( cmuClock_TIMER1, true );

	TIMER1->TOP = 0x3FFF;

	TIMER_Init( TIMER1, &timer1 );
	TIMER1->IEN = TIMER_IEN_OF;
	NVIC_EnableIRQ( TIMER1_IRQn );
}

void TIMER1_IRQHandler(void) {
	TIMER_IntClear(TIMER1, TIMER_IF_OF);
	for( uint8_t i = 0; i < TIMERS_COUNT; i++ ) {
		if( timers[i] ) {
			if( !--timers[i] ) {
				event_set( evTIMER_BASE + i );
			}
		}
	}
}


