#include <lcom/lcf.h>

/** @defgroup utils utils
 * @{
 *
 * utils
 */

#include <stdint.h>

/**
 * @brief Gets LSB from val
 * 
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief Gets MSB from val
 * 
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief calls sys_in()
 * 
 */
int (util_sys_inb)(int port, uint8_t *value);

/**@}*/
