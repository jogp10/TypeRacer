#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id = 2;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) { 

  uint8_t st;

  if (timer_get_conf(timer, &st) != 0) {
    printf("Error getting configuration in timer_set_frequency\n");
    return 1;
  }

  uint8_t ctrlw;
  uint8_t lsb, msb;

  uint16_t hrtz = TIMER_FREQ / freq;

  util_get_LSB(hrtz , &lsb);
  util_get_MSB(hrtz , &msb);

  switch (timer) {
    case 0:
      ctrlw = ( TIMER_SEL0 | TIMER_LSB_MSB | (st & 0x0F));
      sys_outb(TIMER_CTRL, ctrlw);
      sys_outb(TIMER_0, lsb);
      sys_outb(TIMER_0, msb);
      break;
    
    case 1:
      ctrlw = ( TIMER_SEL1 | TIMER_LSB_MSB | (st & 0x0F));
      sys_outb(TIMER_CTRL, ctrlw);
      sys_outb(TIMER_1, lsb);
      sys_outb(TIMER_1, msb);
      break;

    case 2:
      ctrlw = ( TIMER_SEL2 | TIMER_LSB_MSB | (st & 0x0F));
      sys_outb(TIMER_CTRL, ctrlw);
      sys_outb(TIMER_2, lsb);
      sys_outb(TIMER_2, msb);
      break;

    default:
      printf("Error selecting Timer\n");
      return 1;
  }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
  if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id)!=0){
    printf("Error subscribing int\n");
    return 1;
  }
  return 0;
}

int (timer_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id)!=0){
    printf("Error unsubscribing int\n");
    return 1;
  }

  return 0;
}

void (timer_int_handler)() {
  count++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  
  uint8_t read_back = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));

  if (sys_outb(TIMER_CTRL, read_back) != 1) {
    switch (timer) {
      case 0:
        if (util_sys_inb(TIMER_0, st) != 0) {
          printf("Error in TIMER 0.\n");
          return 1;
        }
        break;
      case 1:
        if (util_sys_inb(TIMER_1, st) != 0) {
          printf("Error in TIMER 1.\n");
          return 1;
        }
        break;
      case 2:
        if (util_sys_inb(TIMER_2, st) != 0) {
          printf("Error in TIMER 2.\n");
          return 1;
      }
        break;
      default:
        printf("No timer selection.\n");
        return 1;
    }
    return 0;
  }

  printf("Error! Configuration not found.\n");
  return 1;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  if ( timer < 0 || timer > 2){
    printf("Invalid TIMER REG.\n");
    return 1;
  }

  union timer_status_field_val config;
  uint8_t aux = st;
  switch (field) {
    case tsf_all:
      config.byte=aux;
      break;
    
    case tsf_initial:
      switch (aux & 0x30) {
        case TIMER_LSB:
          config.in_mode=LSB_only;
          break;
        case TIMER_MSB:
          config.in_mode=MSB_only;
          break;
        case TIMER_LSB_MSB:
          config.in_mode=MSB_after_LSB;
          break;
        default:
          config.in_mode=INVAL_val;
          printf("Invalid Initialzation Mode\n");
          return 1;
      }
      break;

    case tsf_mode:
      switch (aux & 0x0E) {
        case 0:
          config.count_mode=0;
          break;
        case 2:
          config.count_mode=1;
          break;
        case 4:
        case 12:
          config.count_mode=2;
          break;
        case 6:
        case 14:
          config.count_mode=3;
          break;
        case 8:
          config.count_mode=4;
          break;
        case 10:
          config.count_mode=5;
          break;
        default:
          printf("Error! Counting Mode incorrect!\n");
          return 1;
      }
      break;

      case tsf_base:
        switch (aux & 0x01) {
        case TIMER_BCD:
          config.bcd = true;
          break;
        case TIMER_BIN:
          config.bcd = false;
          break;
        default:
          printf("Error! Failed to check BCD.\n");
          return 1;
        }
        break;
      
      default:
        printf("Error! Human friendly field.\n");
        return 1;

  }

  if(timer_print_config(timer, field, config) != 0){
    printf("Error in displaying status!");
    return 1;
  }

  return 0;
}
