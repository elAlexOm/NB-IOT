
/** \file 		events.h
 * 	\author		Alex
 * 	\date		11.07.2017
 *
 * 	Описание событий программы
 */

#pragma once

#include <stdint.h>

#define EVENT_COUNT					16

#define evKEY1_ON					0x01
#define evKEY1_OFF					0x02
#define evKEY2_ON					0x03
#define evKEY2_OFF					0x04

#define evUART_RX					0x10
#define evUART_TX_END				0x11
#define evUART_RX_END				0x12

#define evTIMER_BASE				0x20
#define evTIMER_0					( evTIMER_BASE )
#define evTIMER_1					( evTIMER_BASE + 1 )
#define evTIMER_2					( evTIMER_BASE + 2 )
#define evTIMER_3					( evTIMER_BASE + 3 )
#define evTIMER_4					( evTIMER_BASE + 4 )
#define evTIMER_5					( evTIMER_BASE + 5 )
#define evTIMER_6					( evTIMER_BASE + 6 )
#define evTIMER_7					( evTIMER_BASE + 7 )


void event_set( uint8_t event );
uint8_t event_get( void );
uint8_t event_count( void );
