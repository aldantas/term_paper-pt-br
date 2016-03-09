#include "wsn_remote.h"
#include "adc.h"
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define GAS_THRESHOLD 500

volatile uint8_t* LED_DDR = &DDRB;
volatile uint8_t* LED_PORT = &PORTB;
uint8_t LED = PB0;
volatile uint8_t* NRF_PORT = &PORTD;
uint8_t NRF = PD2;
uint8_t TRANSDUCER0 = 'G';
uint8_t TRANSDUCER1 = '0';
uint8_t value[2];

volatile uint16_t gas_value;
volatile uint8_t recent_emergency_counter = 0;

void WSN_mount_measurement_message(void)
{
	message[0] = 'M';
	message[1] = TRANSDUCER0;
	message[2] = 'A';
	memcpy(&message[3], value, 2);
	message[5] = TRANSDUCER1;
	message[6] = '0';
	message[7] = '0';
	message[8] = '0';
}

void WSN_handle_write_message(void) {}

WSN_NRF_interrupt_routine

ISR(ADC_vect)
{
	value[0] = ADCL;
	value[1] = ADCH;
	WSN_push_payload();

	if(recent_emergency_counter) {
		recent_emergency_counter--;
	}

	gas_value = (value[1] << 8) | value[0];

	if(gas_value >= GAS_THRESHOLD && !recent_emergency_counter) {
		WSN_mount_emergency_message(value);
		ignore_next_pkg = 1;
		parent_oriented = 1;
		recent_emergency_counter = 20;
		MIRF_TX;
		MIRF_send_data(parent_address, message, 8);
	}
}

int main(void)
{
	WSN_setup();
	ADC_setup(64, ADC_NORMAL, 1);
	ADC_init();
	ADC_select_channel(ADC5D);

	sei();

	WSN_ingress();

	while(1) {
		ADC_start_conversion();
		_delay_ms(1000);
	}

	return 0;
}
