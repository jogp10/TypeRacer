#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

/** @defgroup keyboard keyboard
 * @{
 *
 * Functions for using the i8042 KBC
 */

/**
 * @brief Subscribes and enables keyboard interrupt handler
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(keyboard_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes keyboard interrupt handler
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(keyboard_unsubscribe_int)();

/**
 * @brief keyboard interrupt handler
 *
 *
 */
void(KBC_IH)();

/**
 * @brief gets the scancodes from the keyboard
 * 
 */
void(get_scancode)();

#endif