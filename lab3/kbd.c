#include <lcom/lcf.h>

#include <stdint.h>

#include "kbd.h"
#include "utils.c"
#include "i8042.h"

uint8_t scan_code; // make code or break code
bool error = false;
int hook_id_k = KEYBOARD_IRQ;
unsigned int count_k = 0;

void (kbd_ih)() {
  read_scancode();
}

int (kbd_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id_k;
  if(sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_k)!=0){
    printf("Error subscribing int.\n");
    return 1;
  }
  return 0;
}

int (kbd_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id_k)!=0){
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
  printf("ONE BYTE\n");
  byte[(*size)-1] = scan_code;
  return 1; 
}

void read_scancode() {
  uint8_t status_reg=false;

  count_k++;
  if (util_sys_inb(STATUS_REG, &status_reg)!=0) {
    printf("Error reading status.\n");
    error = true;
    return;
  }

  if (status_reg & (STAT_REG_OBF|STAT_REG_AUX)) {
    count_k++;
    error = false;
    if (util_sys_inb(OUTPUT_BUF, &scan_code)!=0) {
        printf("Error reading output.\n");
        error = true;
        return;
    }

    if (status_reg & (STAT_REG_PAR|STAT_REG_TIMEOUT)) {
      error = true;
      return;
    }
  } 
  else error = true;
}

int enable_kbd_interrupts() {
  uint8_t cmd_byte = 0x00;

  sys_outb(STATUS_REG, READ_CMD_BYTE);
  util_sys_inb(INPUT_BUF, &cmd_byte);

  cmd_byte = cmd_byte | ENABLE_INTERRUPT;

  sys_outb(STATUS_REG, WRITE_CMD_BYTE);
  sys_outb(OUTPUT_BUF, cmd_byte);

	return 1;
}
