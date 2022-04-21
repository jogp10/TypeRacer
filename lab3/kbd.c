#include <lcom/lcf.h>
#include "kbd.h"

#include "i8042.h"

unsigned int counter_kbd;
int hook_id = KBD_IRQ;
bool error;
uint8_t code;

int (kbc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
  if(sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)) {
    printf("Error subscribing kbc interruption.\n");
    return 1;
  }

  return 0;
}

int (kbc_unsubscribe_int)(uint8_t *bit_no) {
  if(sys_irqrmpolicy(&hook_id)) {
    printf("Error unsubscribing timer interruption.\n");
    return 1;
  };
  return 0;
}

void (kbc_ih)() {
  uint8_t status;
  error = true;

  while( error ) {
    error = true;

    counter_kbd++;
    /* Read status register */
    if (util_sys_inb(KBC_ST_REG, &status)) {
      printf("Error reading status register.\n");
      error = true;
      return;
    }

    /* loop while 8042 output buffer is empty */
    if( status & (KBC_OBF) ) {

      counter_kbd++;
      if (util_sys_inb(KBC_OUT_BUF, &code)) { /* read output buffer */
        error = true;
        printf("Error reading output buffer.\n");
        return;
      }

      if ( (status & (KBC_PAR_ERR | KBC_TO_ERR)) ){ /* parity or timeout error */
        error = true;
        printf("Error parity or timeout error.\n");
        return;
      } else error = false;

      return;
    }

    TIME_DELAY; // e.g. tickdelay()
  }

}

bool(kbc_code_complete) (uint8_t scan_code[], uint8_t *size) {
  if (code == TWO_BYTE) {
    scan_code[0] = TWO_BYTE;
    *size = 2;
    return false;
  }
  scan_code[(*size)-1] = code;
  return true;
}
