#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>
#include <stdint.h>

#include "i8042.h"
#include "util.h"

void (kbd_ih)();

int (kbd_subscribe_int)(uint8_t *bit_no);

int (kbd_unsubscribe_int)();

int (kbd_scancode_complete) (uint8_t byte[], uint8_t *size);

#endif /*_LCOM_KBC_H_ */
