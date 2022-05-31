#ifndef _LCOM_VC_MACROS_H_
#define _LCOM_VC_MACROS_H_

#include <lcom/lcf.h>
#include <minix/sysutil.h>

/** @defgroup VC MACROS VC MACROS
 * @{
 *
 * Constants for programming the video graphics.
 */

/**@}*/

#define TO_SECONDS(n)   n * 1000000
#define TIME_DELAY(delay)    tickdelay(micros_to_ticks(TO_SECONDS(delay)))

#define VBE_INT         0x10
#define VBE_CALL        0x4F
#define VBE_SUPPORTED   0x4F
#define VBE_SUCCESS     0x00
#define VBE_VIDEO       BIT(14)
#define VBE_VG          0x02
#define VBE_TEXT        0x03

#define DIRECT_COLOR_MODEL_1 6
#define DIRECT_COLOR_MODEL_2 7

#define INDEXED_COLOR   0x105

#define MODE1 0x105

#endif /* _LCOM_VC_MACROS_H_ */
