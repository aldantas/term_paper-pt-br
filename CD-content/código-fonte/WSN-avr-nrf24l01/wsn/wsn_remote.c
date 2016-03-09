#include "wsn_remote.h"
#include <util/delay.h>

uint8_t address[ADDR_SIZE+1] = "Pnnode";

void WSN_setup(void)
{
	MIRF_setup_config();
	MIRF_enable_rx_pipe(ERX_P0, 0, BROADCAST_ADDR);
	MIRF_enable_rx_pipe(ERX_P1, 0, &address[1]);
	MIRF_enable_ack_payload();
	MIRF_init();

	set_bit(*LED_DDR, LED);

	MIRF_RX;

	set_bit(*NRF_PORT, NRF);
	EINT_enable(INT0, EISC_LOW_LEVEL);
}

void WSN_push_payload(void)
{
	WSN_mount_measurement_message();
	MIRF_flush_tx();
	MIRF_write_ack_payload(0, address, 6);
	MIRF_write_ack_payload(1, message, 9);
}

void WSN_mount_new_node_message(void)
{
	message[0] = 'N';
	message[1] = TRANSDUCER0;
	message[2] = TRANSDUCER1;
	memcpy(&message[3], &address[1], ADDR_SIZE);
}

void WSN_mount_new_node_foward_message(void)
{
	memcpy(message, &address[1], ADDR_SIZE);
	memcpy(&message[ADDR_SIZE], data, payload_size);
	payload_size += ADDR_SIZE;
}

void WSN_mount_emergency_message(uint8_t* value)
{
	message[0] = 'E';
	memcpy(&message[1], &address[1], ADDR_SIZE);
	memcpy(&message[ADDR_SIZE+1], value, 2);
}

void WSN_ingress(void)
{
	_delay_ms(10);
	WSN_mount_new_node_message();
	MIRF_TX;
	MIRF_send_data(MASTER_ADDR, message, ADDR_SIZE + 3);
	_delay_ms(50);
	if(parent_address[0]) return;
	MIRF_send_data(BROADCAST_ADDR, (uint8_t*)"R", 1);
	_delay_ms(50);
	if(!parent_address[0]) {
		WSN_node_shutdown();
	}
}

void WSN_node_shutdown(void)
{
	set_bit(*LED_PORT, LED);
	MIRF_POWER_DOWN;
}
