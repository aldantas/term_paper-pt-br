#include "wsn_remote.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RELAY_DDR DDRA
#define RELAY_PORT PORTA
#define RELAY PA2

volatile uint8_t* LED_DDR = &DDRA;
volatile uint8_t* LED_PORT = &PORTA;
uint8_t LED = PA1;
volatile uint8_t* NRF_PORT = &PORTB;
uint8_t NRF = PB2;
uint8_t TRANSDUCER0 = 'R';
uint8_t TRANSDUCER1 = '0';
uint8_t relay_status = 0;

void WSN_mount_measurement_message(void)
{
	message[0] = 'M';
	message[1] = TRANSDUCER0;
	message[2] = 'D';
	message[3] = 0;
	message[4] = relay_status;
	message[5] = TRANSDUCER1;
	message[6] = '0';
	message[7] = '0';
	message[8] = '0';
}

void WSN_handle_write_message(void) {
	if(data[1] == '1') {
		set_bit(RELAY_PORT, RELAY);
		relay_status = 1;
	} else if(data[1] == '0') {
		clear_bit(RELAY_PORT, RELAY);
		relay_status = 9;
	}
}

WSN_NRF_interrupt_routine

int main(void)
{
	WSN_setup();

	set_bit(RELAY_DDR, RELAY);

	sei();

	WSN_ingress();

	while(1) {
	}

	return 0;
}
