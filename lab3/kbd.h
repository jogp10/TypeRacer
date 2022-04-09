#ifndef __KBD_H_
#define __KBD_H_

#include <lcom/lcf.h>
#include <stdint.h>

/** @defgroup kbd kbd
 * @{
 *
 * Functions for using the i8042 kbd
 */

/**
 * @brief KBD interrupt handler
 *
 * 
 */
void (kbd_ih)();

/**
 * @brief Subscribes and enables KBD interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes KBD interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_unsubscribe_int)();

int(kbd_scancode_complete) (uint8_t byte[], uint8_t *size);

void(read_scancode)();

int(enable_kbd_interrupts) ();

#endif /*__KBD_H_ */
