#ifndef WSN_MASTER
#define WSN_MASTER

#include "wsn.h"
#include "ext_interrupt.h"
#include "usart.h"
#include <avr/io.h>

void WSN_send_message(uint8_t* message, uint8_t message_size);

#define WSN_NRF_interrupt_routine \
ISR(INT0_vect) \
{ \
	if(MIRF_MAX_RT_REACHED) { \
		MIRF_CLEAR_MAX_RT; \
		ignore_next_pkg = 0; \
		WSN_mount_fail_message(); \
		USART_transmit_byte_array(message, ADDR_SIZE+1); \
		MIRF_set_address(ERX_P0, &address[1]); \
		MIRF_RX; \
	} \
	if(MIRF_DATA_SENT) { \
		MIRF_CLEAR_TX_DS; \
		MIRF_set_address(ERX_P0, &address[1]); \
		WSN_push_payload(); \
		if(ignore_next_pkg) { \
			ignore_next_pkg = 0; \
			MIRF_flush_rx(); \
			MIRF_CLEAR_RX_DR; \
		} \
		MIRF_RX; \
	} \
	if(MIRF_DATA_READY) { \
		payload_size = MIRF_read_dynamic_payload_data(message); \
		MIRF_CLEAR_RX_DR; \
		USART_transmit_byte_array(message, payload_size); \
		if(WSN_is_new_node_message(message, payload_size)) { \
			mount_and_send_address_package(); \
		} else if(message[0] == 'M') { \
			if(message[2] == 'A') { \
				brightness = (message[4] << 8) | message[3]; \
				USART_print_string("\n"); \
				USART_print_word(brightness); \
			} else { \
				USART_print_string("\n"); \
				USART_print_word(message[4]); \
				USART_print_string("\n"); \
				USART_print_word(message[8]); \
			} \
		} \
	} \
}

#define WSN_USART_interrupt_routine \
ISR(USART_RX_vect) \
{ \
	payload_size = USART_receive_byte_array_until(message, '\\'); \
	WSN_send_message(message, payload_size); \
}

#endif
