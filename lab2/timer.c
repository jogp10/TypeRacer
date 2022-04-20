#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  uint8_t status;

  if(timer_get_conf(timer, &status)) {
      printf("Error getting timer config.\n");
      return 1;
  }

  uint8_t control_word, timer_port;
  uint8_t lsb, msb;

  uint16_t hrtz = TIMER_FREQ / freq;
  // dividi hrtz into 8 bit vars
  util_get_LSB(hrtz, &lsb);
  util_get_MSB(hrtz, &msb);
  
  // Mount control word
  switch(timer) {
    case 0:
      control_word = TIMER_SEL0;
      timer_port = TIMER_0;
      break;
    case 1:
      control_word = TIMER_SEL1;
      timer_port = TIMER_1;
      break;
    case 2:
      control_word = TIMER_SEL2;
      timer_port = TIMER_2;
      break;
    default:
      printf("Invalid timer.\n");
      return 1;
  }

  control_word = control_word | TIMER_LSB_MSB | (status & 0x0F);
  sys_outb(TIMER_CTRL, control_word);

  // Load hrtz to timer port
  sys_outb(timer_port, lsb);
  sys_outb(timer_port, msb);

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if(!(timer>=0  && timer<=2)) {
    printf("Error invalid timer.\n");
    return 1;
  }
  
  // Mount Back Command and write to timer control register
  uint8_t read_back_cmmd = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;
  if(sys_outb(TIMER_CTRL, read_back_cmmd)) {
    printf("Error writing to timer control reg.\n");
    return 1;
  };
  
  uint8_t timer_port;
  switch(timer) {
    case 0:
      timer_port = TIMER_0;
      break;
    case 1:
      timer_port = TIMER_1;
      break;
    case 2:
      timer_port = TIMER_2;
      break;
    default:
      return 1;
  }

  // Read config
  if(util_sys_inb(timer_port, st)) {
    printf("Error reading timer config.\n");
    return 1;
  };

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  if(!(timer>=0  && timer<=2)) {
    printf("Error invalid timer.\n");
    return 1;
  }

  uint8_t count_mode;
  union timer_status_field_val conf;

  switch (field) {
    case tsf_all:
      conf.byte=st;
      break;

    case tsf_base:
      switch(st & 0x1) {
        case TIMER_BCD:
          conf.bcd=true;
          break;
        case TIMER_BIN:
          conf.bcd=false;
          break;
        default:
          printf("Invalid base.\n");
          return 1;
      }
      break;

    case tsf_initial:
      switch(st & 0x30) {
        case TIMER_LSB:
          conf.in_mode = LSB_only;
          break;
        case TIMER_MSB:
          conf.in_mode = MSB_only;
          break;
        case TIMER_LSB_MSB:
          conf.in_mode = MSB_after_LSB;
          break;
        default:
          conf.in_mode = INVAL_val;
          return 1;
      }
      break;

    case tsf_mode:
      count_mode = (st & 0xE) >> 1;
      if(count_mode>5 || count_mode<0) {
        printf("Incorrect count mode.\n");
        return 1;
      }
      conf.count_mode = count_mode;
      break;
      
    default:
      printf("Incorrect field selection.\n");
      return 1;
  }

  if(timer_print_config(timer, field, conf)) {
    printf("Error printing timer conf.\n");
    return 1;
  };

  return 0;
}
