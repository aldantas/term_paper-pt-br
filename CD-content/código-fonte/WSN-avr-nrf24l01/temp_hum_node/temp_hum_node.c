#include "wsn_remote.h"
#include "dht11.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t* LED_DDR = &DDRA;
volatile uint8_t* LED_PORT = &PORTA;
uint8_t LED = PA1;
volatile uint8_t* NRF_PORT = &PORTB;
uint8_t NRF = PB2;
uint8_t TRANSDUCER0 = 'H';
uint8_t TRANSDUCER1 = 'T';
uint8_t value[2];

void WSN_mount_measurement_message(void)
{
	message[0] = 'M';
	message[1] = TRANSDUCER0;
	message[2] = 'D';
	message[3] = 0;
	message[4] = value[0];
	message[5] = TRANSDUCER1;
	message[6] = 'D';
	message[7] = 0;
	message[8] = value[1];
}

void WSN_handle_write_message(void) {}

WSN_NRF_interrupt_routine

int main(void)
{
	WSN_setup();

	sei();

	WSN_ingress();

	while(1) {
		DHT11_get_data(value);
		WSN_push_payload();
		_delay_ms(5000);
	}

	return 0;
}
