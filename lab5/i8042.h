#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>
#include <minix/sysutil.h>

/** @defgroup i8042 i8042
 * @{
 * Constants for programming the i8042 Keyboard. 
 */

#define KBD_IRQ       1 /**< @brief Keyboard IRQ line */

#define DELAY_US      2000
#define TIME_DELAY    tickdelay(micros_to_ticks(DELAY_US))

/* Makecodes and Breakcodes */

#define ESC_MAKE      0x01
#define ESC_BREAK     0x81
#define TWO_BYTE      0xe0
#define MAKE_CODE     BIT(7)

/* Keyboard registers */

#define KBC_IN_BUF    0x60
#define KBC_OUT_BUF   0x60
#define KBC_ST_REG    0x64
#define CMMD_B_REG    0x60

/* KBC Commands */

#define CMMD_B_READ   0x20
#define CMMD_B_WRITE  0x60
#define ENABLE_INT    0x01

/* KBC Command byte */

#define KBC_PAR_ERR   BIT(7)
#define KBC_TO_ERR    BIT(6)
#define KBC_AUX       BIT(5)
#define KBC_OBF       BIT(0)



/**@}*/

#endif /* _LCOM_I8042_H_ */
