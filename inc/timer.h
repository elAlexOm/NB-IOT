/*
 * 	\file			timer.h
 *	\author			Alex
 * 	\date 			12 июля 2017 г.2017
 */

#pragma once

#define TIMERS_COUNT						8

#define tmrKEYS								timers[0]
#define tmrRX_MESS							timers[1]

extern uint32_t timers[];

#define START_TIMER( TIMER, TIMEOUT )		{ TIMER = TIMEOUT; }

void init_timers( void );
