#include "kbd.h"

uint8_t scan_code; // make code or break code
bool error = false;
int hook_id = KEYBOARD_IRQ;
unsigned int count = 0;

void (kbd_ih)() {
  read_scancode();
}

int (kbd_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
  if(sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)!=0){
    printf("Error subscribing int.\n");
    return 1;
  }
  return 0;
}

int (kbd_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id)!=0){
    printf("Error unsubscribing int.\n");
    return 1;
  }

  return 0;
}

int (kbd_scancode_complete) (uint8_t byte[], uint8_t *size) {
  if(error) {
    printf("Error\n");
    return 0;
  }

  if(scan_code == TWO_BYTE_SCANCODE) {
    byte[(*size)-1] = TWO_BYTE_SCANCODE;
    *size = 2;
    printf("TWO BYTE\n");
    return 0;
  }

  byte[(*size)-1] = scan_code;
  return 1; 
}

void read_scancode() {
  uint8_t status_reg=false;

  count++;
  if (util_sys_inb(STATUS_REG, &status_reg)!=0) {
    printf("Error reading status.\n");
    return;
  }

  if ((status_reg & STAT_REG_OBF) && !(status_reg & (STAT_REG_PAR|STAT_REG_TIMEOUT|STAT_REG_AUX))) {
    //Data available for reading and no Errors
    count++;
    if (util_sys_inb(OUTPUT_BUF, &scan_code)!=0) {
      printf("Error reading output.\n");
      return;
    }
  } else {
    error = true;
    return;
  }
  return;
}

int enable_kbd_interrupts() {
	return 1;
}
