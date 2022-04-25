#ifndef __KBD_H
#define __KBD_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Subscribes and enables KBC interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes KBC interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_unsubscribe_int)();

/**
 * @brief KBC interrupt handler
 *
 * Read scan code
 */
void(kbc_ih)();

/**
 * @brief Checks code if completed
 *
 * @return Return true upon completion and false otherwise 
 */
bool(kbc_code_complete)(uint8_t scan_code[], uint8_t *size);

/**
 * @brief Reenable kbc interrupts
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_reenable_int)();

#endif /* __KBD_H */
