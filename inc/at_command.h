/*
 * 	\file			at_command.h
 *	\author			Alex
 * 	\date 			12 июля 2017 г.2017
 */

#pragma once

#define AT_RX_BUFFER_LENGTH				256

typedef enum {
	AT_ERROR 	= 0,
	AT_OK		= 1
} at_status_t;

at_status_t at_sent( char* command );

at_status_t at_receive( void );
