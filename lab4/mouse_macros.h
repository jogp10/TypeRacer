#ifndef _LCOM_MOUSE_MACROS_H_
#define _LCOM_MOUSE_MACROS_H_

#include <lcom/lcf.h>
#include <minix/sysutil.h>

/** @defgroup MOUSE MACROS MOUSE MACROS
 * @{
 *
 * Constants for programming the mouse.
 */

/**@}*/

#define MOUSE_IRQ = 12 /**< @brief Mouse IRQ line */

#define DELAY_US      2000
#define TIME_DELAY    tickdelay(micros_to_ticks(DELAY_US))

#define FIRST_PACKET BIT(3)

/* Packet struct */
#define LB      BIT(0)
#define RB      BIT(1)
#define MB      BIT(2)
#define X_SIGN  BIT(4)
#define Y_SIGN  BIT(5)
#define X_OVF   BIT(6)
#define Y_OVF   BIT(7)

#endif /* _LCOM_MOUSE_MACROS_H_ */
