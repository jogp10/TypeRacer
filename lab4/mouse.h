#ifndef __MOUSE_H
#define __MOUSE_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Subscribes and enables Mouse interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Mouse interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_unsubscribe_int)();

int (mouse_enable_int)();
int (mouse_disable_int)();

int (mouse_issue_command)(uint8_t cmd);

int (mouse_set_stream_mode)();
int (mouse_enable_data_rep)();
int (mouse_disable_data_rep)();

/**
 * @brief Mouse interrupt handler
 *
 * Read packet code
 */
void (mouse_ih)();

/**
 * @brief Checks packet if completed
 *
 * @return Return true upon completion and false otherwise 
 */
bool (mouse_packet_complete)(uint8_t packets[], uint8_t *size);

int (build_packet_struct)(uint8_t packets[], struct packet *pp);



#endif /* __MOUSE_H */
