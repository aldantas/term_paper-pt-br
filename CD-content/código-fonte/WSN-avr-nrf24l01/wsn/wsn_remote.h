#ifndef WSN_REMOTE
#define WSN_REMOTE

#include "wsn.h"
#include "ext_interrupt.h"
#include <avr/io.h>

extern volatile uint8_t* LED_DDR;
extern volatile uint8_t* LED_PORT;
extern uint8_t LED;
extern uint8_t TRANSDUCER0;
extern uint8_t TRANSDUCER1;

uint8_t parent_address[ADDR_SIZE];
uint8_t parent_oriented;

void WSN_mount_new_node_message(void);

void WSN_mount_new_node_foward_message(void);

void WSN_mount_emergency_message(uint8_t* value);

void WSN_ingress(void);

void WSN_node_shutdown(void);

extern void WSN_mount_measurement_message(void);

extern void WSN_handle_write_message(void);

#define WSN_NRF_interrupt_routine \
ISR(INT0_vect) \
{ \
	if(MIRF_MAX_RT_REACHED) { \
		MIRF_CLEAR_MAX_RT; \
		if(parent_address[0]) { \
			if(parent_oriented) { \
				WSN_node_shutdown(); \
			} else { \
				ignore_next_pkg = 1; \
				parent_oriented = 1; \
				WSN_mount_fail_message(); \
				MIRF_TX; \
				MIRF_send_data(parent_address, message, 6); \
				return; \
			} \
		} \
	} \
	if(MIRF_DATA_SENT) { \
		MIRF_set_address(ERX_P0, BROADCAST_ADDR); \
		MIRF_CLEAR_TX_DS; \
		if(ignore_next_pkg) { \
			ignore_next_pkg = 0; \
			MIRF_CLEAR_RX_DR; \
			MIRF_flush_rx(); \
		} \
		WSN_push_payload(); \
		MIRF_RX; \
	} \
	if(MIRF_DATA_READY) { \
		payload_size = MIRF_read_dynamic_payload_data(data); \
		MIRF_CLEAR_RX_DR; \
		switch (data[0]) { \
			case 'A': \
				memcpy(&address[1], &data[1], ADDR_SIZE); \
				MIRF_set_address(ERX_P1, &address[1]); \
				WSN_push_payload(); \
				break; \
			case 'E': \
			case 'F': \
			case 'M': \
				ignore_next_pkg = 1; \
				parent_oriented = 1; \
				MIRF_TX; \
				MIRF_send_data(parent_address, data, payload_size); \
				break; \
			case 'P': \
				memcpy(parent_address, &data[1], ADDR_SIZE); \
				WSN_push_payload(); \
				WSN_mount_new_node_message(); \
				ignore_next_pkg = 1; \
				parent_oriented = 1; \
				MIRF_TX; \
				MIRF_send_data(parent_address, message, ADDR_SIZE + 3); \
				break; \
			case 'R': \
				WSN_push_payload(); \
				break; \
			case 'W': \
				WSN_handle_write_message(); \
				WSN_push_payload(); \
				break; \
			default: \
				if(WSN_is_new_node_message(data, payload_size)) { \
					if(payload_size <= 23) { \
						ignore_next_pkg = 1; \
						parent_oriented = 1; \
						WSN_mount_new_node_foward_message(); \
						MIRF_TX; \
						MIRF_send_data(parent_address, message, payload_size); \
					} \
				} else { \
					memcpy(last_sent_address, data, ADDR_SIZE); \
					if(!WSN_is_direct_read(&data[5])) { \
						ignore_next_pkg = 1; \
					} \
					parent_oriented = 0; \
					MIRF_TX; \
					MIRF_send_data(data, &data[5], payload_size - 5); \
				} \
		} \
	} \
}

#endif
