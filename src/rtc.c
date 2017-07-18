/*
 * 	\file				rtc.c
 *	\author			Alex
 * 	\date 			18 июля 2017 г.2017
 */

#include "rtc.h"
#include "em_rtc.h"
#include "em_cmu.h"

#include "events.h"

void init_rtc( void ) {
	RTC_Init_TypeDef rtcInit = {
		.debugRun = false,
		.comp0Top = true,
		.enable   = true,
	};

	CMU_OscillatorEnable( cmuOsc_LFXO, true, true );
  CMU_ClockSelectSet( cmuClock_LFA, cmuSelect_LFXO );
  CMU_ClockEnable(cmuClock_CORELE, true);

  // Input RTC init struct in initialize function
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_32768); //cmuClkDiv_32

  // Enable the RTC clock
  CMU_ClockEnable(cmuClock_RTC, true);

  // Use internal crystal
  RTC_Reset();
  RTC_Init( &rtcInit );

  // No start if > T8mn just Tick timer 8mn
  RTC_CompareSet(0, 0xFFFFFF);

  // No start if > T8mn just Tick timer 8mn
//  RTC_CompareSet( 1, 0xFFFFFF );
  RTC_CompareSet( 1, IRQ_PERIOD - 1 ); // 1sec interrupt
  RTC_IntClear(RTC_IFC_COMP1);
  RTC_IntEnable(RTC_IF_COMP1);

  // Enable RTC interrupt vector in NVIC
  NVIC_EnableIRQ(RTC_IRQn);

  // Enable the RTC timer
  RTC_Enable(true);
}

void RTC_IRQHandler(void) {
  static uint8_t counter = 0x40;
  uint32_t flag = RTC_IntGet();

  if( flag & RTC_IFC_OF ) {
  	RTC_IntClear( RTC_IF_OF );
  }
  if( flag & RTC_IFC_COMP0 ) {
  	RTC_IntClear( RTC_IF_COMP0 );

  }
  if( flag & RTC_IFC_COMP1 ) {
  	RTC_IntClear( RTC_IF_COMP1 );
  	RTC_CounterReset();

  	event_set( evSECOND );
  }
}
