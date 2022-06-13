#ifndef __KBD_H
#define __KBD_H

#include <stdbool.h>
#include <stdint.h>

/** @defgroup keyboard keyboard
 * @{
 *
 * keyboard module
 */

/**
 * @brief Subscribes and enables KBC interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes KBC interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_unsubscribe_int)();

/**
 * @brief KBC interrupt handler
 *
 * Read scan code
 */
void(kbd_ih)();

/**
 * @brief Checks code if completed
 *
 * @return Return true upon completion and false otherwise 
 */
bool(kbd_code_complete)(uint8_t scan_code[], uint8_t *size);

/**
 * @brief Reenable kbc interrupts
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_reenable_int)();

/**
 * @brief 
 * 
 * @param cmd command to be issued
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_issue_command)(uint8_t cmd);


int (kbc_issue_command_with_arg)(uint8_t arg);

/**
 * @brief Reads acknowledgemnt byte
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_read_acknowledgment)(uint8_t *acknowledgment_byte);

/**@}*/

#endif /* __KBD_H */
