#include "wsn_master.h"
#include <util/delay.h>

uint8_t address[ADDR_SIZE+1] = "Pmastr";

void WSN_setup(void)
{
	MIRF_setup_config();
	MIRF_enable_rx_pipe(ERX_P0, 0, &address[1]);
	MIRF_enable_ack_payload();
	MIRF_init();

	WSN_push_payload();
	MIRF_RX;

	set_bit(*NRF_PORT, NRF);
	EINT_enable(INT0, EISC_LOW_LEVEL);

	USART_init();
	USART_enable_rx_interrupt;
	USART_print_string("Ready!");
}

void WSN_push_payload(void)
{
	MIRF_flush_tx();
	MIRF_write_ack_payload(0, address, 6);
}

void WSN_send_message(uint8_t *message, uint8_t message_size)
{
	if(message_size > ADDR_SIZE) {
		memcpy(last_sent_address, message, ADDR_SIZE);
		uint8_t *dest_addr = message;
		uint8_t *payload = &message[5];
		if(!WSN_is_direct_read(payload)) {
			ignore_next_pkg = 1;
		}
		MIRF_TX;
		MIRF_send_data(dest_addr, payload, message_size - ADDR_SIZE);
	}
}

