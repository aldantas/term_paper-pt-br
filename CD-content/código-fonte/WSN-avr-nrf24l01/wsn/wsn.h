#ifndef WSN
#define WSN

#include "mirf.h"
#include "macros.h"
#include <string.h>
#include <avr/io.h>

#define MAX_MSG_SIZE 34

#define BROADCAST_ADDR (uint8_t*)"broad"
#define MASTER_ADDR (uint8_t*)"mastr"

extern volatile uint8_t* NRF_PORT;
extern uint8_t NRF;

uint8_t data[MAX_MSG_SIZE];
uint8_t message[MAX_MSG_SIZE];
uint8_t payload_size;
extern uint8_t address[ADDR_SIZE+1];
uint8_t last_sent_address[ADDR_SIZE];
extern uint8_t ignore_next_pkg;

void WSN_mount_fail_message(void);

uint8_t WSN_is_new_node_message(uint8_t *message, uint8_t message_size);

uint8_t WSN_is_direct_read(uint8_t* message);

extern void WSN_setup(void);

extern void WSN_push_payload(void);

#endif
