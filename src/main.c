#include "em_device.h"
#include "em_chip.h"

#include "InitDevice.h"

#include "uart.h"
#include "events.h"
#include "fobos_nbiot_bsp.h"
#include "timer.h"
#include "at_command.h"
#include "setting.h"

int main(void)
{
  CHIP_Init();

  enter_DefaultMode_from_RESET();

  init_board();
  init_timers();
  init_uart();

  while (1) {
	  if( event_count() > 0 ) {
		  uint8_t new_event = event_get();

		  switch( new_event ) {
		  	  case evTIMER_0 : {
		  		  key_event();
		  		  break;
		  	  }
		  	  case evTIMER_1 : {
		  		  at_receive();
		  		  break;
		  	  }
		  	  case evTIMER_2 : {
		  		  break;
		  	  }
		  	  case evTIMER_3 : {
		  		  break;
		  	  }
		  	  case evTIMER_4 : {
		  		  break;
		  	  }
		  	  case evTIMER_5 : {
		  		  break;
		  	  }
		  	  case evTIMER_6 : {
		  		  break;
		  	  }
		  	  case evTIMER_7 : {
		  		  break;
		  	  }
		  	  case evKEY1_ON : {
		  		  uart_clear_rx_buffer();
		  		  uart_clear_tx_buffer();
		  		  uart_send( "AT+CGMI\r\n", 9 );
		  		  break;
		  	  }
		  	  case evKEY1_OFF : {
		  		  break;
		  	  }
		  	  case evKEY2_ON : {
		  		  GPIO_PinOutToggle( OPTO_TX_PORT, OPTO_TX_PIN );
		  		  if( GPIO_PinInGet( OPTO_TX_PORT, OPTO_TX_PIN )) {
		  			  GPIO_PinOutClear( SARA_RST_PORT, SARA_RST_PIN );
		  		  }
		  		  else {
		  			  GPIO_PinOutSet( SARA_RST_PORT, SARA_RST_PIN );
		  		  }
		  		  break;
		  	  }
		  	  case evKEY2_OFF : {
		  		  break;
		  	  }
		  	  case evUART_RX : {
		  		  break;
		  	  }
		  	  case evUART_TX_END : {
		  		  break;
		  	  }
		  	  case evUART_RX_END : {

		  		  break;
		  	  }
		  }
	  }
  }
}
