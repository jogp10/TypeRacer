#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"

unsigned int count = 0;
uint8_t scan_code = 0; // make code or break code
int hook_id = 2;

void (kbc_ih)() {
    uint8_t status_reg; 

    count++;
    if (util_sys_inb(STATUS_REG, status_reg)!=0) {
        return;
    }

    if ((status_reg & STAT_REG_OBF) && !(status_reg & (STAT_REG_PAR|STAT_REG_TIMEOUT))) {
        // No error and data available for reading
        util_sys_inb(OUTPUT_BUF, scan_code);
    }
    return;
}

int (kbc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
  if(sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)!=0){
    printf("Error subscribing int.\n");
    return 1;
  }
  return 0;
}

int (kbc_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id)!=0){
    printf("Error unsubscribing int.\n");
    return 1;
  }

  return 0;
}