#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>
#include <minix/sysutil.h>

/** @defgroup i8042 i8042
 * @{
 * Constants for programming the i8042 Keyboard. 
 */

#define KBD_IRQ       1 /**< @brief Keyboard IRQ line */
#define MOUSE_IRQ     12 /**< @brief Mouse IRQ line */

#define DELAY_US      2000
#define TIME_DELAY(n)    tickdelay(micros_to_ticks(n))
#define MAX_TRIES      5

/* Makecodes and Breakcodes */

#define ESC_MAKE      0x01
#define ESC_BREAK     0x81
#define RSHIFT_MAKE   0x36
#define RSHIFT_BRAKE   0xb6
#define LSHIFT_MAKE   0x2a
#define LSHIFT_BRAKE   0xaa
#define CAPS_MAKE 0x3a
#define CAPS_BRAKE 0xba
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

/* KBC STATUS REGISTER byte */

#define KBC_PAR_ERR   BIT(7)
#define KBC_TO_ERR    BIT(6)
#define KBC_AUX       BIT(5)
#define KBC_IBF       BIT(1)
#define KBC_OBF       BIT(0)

/* KBC COMMAND BYTE */

#define KBC_ENABLE_MOUSE_INT BIT(1)
#define KBC_ENABLE_KBD_INT BIT(0)


/* Mouse */

#define FIRST_PACKET BIT(3)

/* Packet struct */
#define LB      BIT(0)
#define RB      BIT(1)
#define MB      BIT(2)
#define X_SIGN  BIT(4)
#define Y_SIGN  BIT(5)
#define X_OVF   BIT(6)
#define Y_OVF   BIT(7)

//PS/2 Mouse Commands

#define MC_RESET       0xFF    /**<@brief Mouse Reset*/
#define MC_RESEND      0xFE    /**<@brief For serial communications errors*/
#define MC_DEFAULTS    0xF6    /**<@brief Set default values*/
#define MC_DIS_DATA_REP    0xF5    /**<@brief In stream mode, Disable (Data Reporting) */
#define MC_EN_DATA_REP       0xF4    /**<@brief In stream mode Enable (Data Reporting) */
#define MC_SET_SR      0xF3    /**<@brief Set Sample Rate*/
#define MC_SET_REMOTE_MODE      0xF0    /**<@brief Set Remote mode */
#define MC_READ_DATA      0xEB    /**<@brief Read Data*/
#define MC_SET_STREAM_MODE      0xEA    /**<@brief Set Stream Mode*/
#define MC_ST_REQ      0xE9    /**<@brief Status Request Get mouse configuration (3 bytes)*/
#define MC_SET_RES     0xE8    /**<@brief Set Resolution*/ 
#define MC_SET_RES21   0xE7    /**<@brief Acceleration mode*/
#define MC_SET_RES11   0xE6    /**<@brief Linear mode*/
#define MC_WRITE        0xD4

//Mouse Controller Acknowledgment Byte
#define MC_ACK         0xFA    /**<@brief Everything OK*/
#define MC_NACK        0xFE    /**<@brief Invalid Byte (may be because of a serial communication error)*/
#define MC_ERROR       0xFC    /**<@brief Second consecutive invalid byte*/


/**@}*/

#endif /* _LCOM_I8042_H_ */
