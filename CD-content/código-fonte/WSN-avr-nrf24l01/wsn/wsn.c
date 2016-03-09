#include "wsn.h"

uint8_t ignore_next_pkg = 0;

void WSN_mount_fail_message(void)
{
	message[0] = 'F';
	memcpy(&message[1], last_sent_address, ADDR_SIZE);
}

uint8_t WSN_is_new_node_message(uint8_t *message, uint8_t message_size)
{
	return message[message_size - ADDR_SIZE - 3] == 'N';
}

uint8_t WSN_is_direct_read(uint8_t* message)
{
	return message[0] == 'R';
}
