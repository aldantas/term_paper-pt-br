#include "wsn_master.h"
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t* NRF_PORT = &PORTD;
uint8_t NRF = PD2;
uint8_t generated_addres[ADDR_SIZE] = "nrf00";
volatile uint16_t brightness;

void mount_and_send_address_package(void)
{
	uint8_t temp[MAX_MSG_SIZE];
	uint8_t new_message_size;
	memcpy(temp, &message[payload_size - ADDR_SIZE], ADDR_SIZE);
	memcpy(&message[payload_size - ADDR_SIZE - 3], temp, ADDR_SIZE);
	message[payload_size - 3] = 'A';
	memcpy(&message[payload_size - 2], generated_addres, ADDR_SIZE);
	new_message_size = payload_size + (ADDR_SIZE - 2);
	USART_print_string("\n");
	USART_transmit_byte_array(message, new_message_size);
	generated_addres[4] += 1;
	ignore_next_pkg = 1;
	WSN_send_message(message, new_message_size);
}

WSN_NRF_interrupt_routine

WSN_USART_interrupt_routine

int main(void)
{
	WSN_setup();

	sei();

	while(1) {
	}

	return 0;
}
