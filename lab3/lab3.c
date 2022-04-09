#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "timer.c"
#include "kbd.h"
#include "i8042.h"

extern uint8_t scan_code;
extern unsigned int count_k;
extern unsigned int count_t;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  int ipc_status, r;
  message msg;

  uint8_t size=1, byte[2];
  count_k = 0;

  // Subscribe kbc interrupts
  if (kbd_subscribe_int(&bit_no)!=0) {
    printf("Error subscribing kbc interrupts.\n");
    return 1;
  }

  while (scan_code!=ESC_BREAK) {
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt*/
            kbd_ih();
          
            if(kbd_scancode_complete(byte, &size)) {
              kbd_print_scancode(!(scan_code & MAKE_CODE), size, byte);
              size = 1;
            }
          }
          break;
      }
    }
  }

  if (kbd_unsubscribe_int()!=0) {
    printf("Error unsubscribing kdb interrupts.\n");
    return 1;
  }

  kbd_print_no_sysinb(count_k);

  return 0;
}

int(kbd_test_poll)() {
  count_k = 0;
  uint8_t size=0, byte[2];

  while (scan_code != ESC_BREAK) {
    read_scancode();

    if(kbd_scancode_complete(byte, &size)) {
      kbd_print_scancode(!(scan_code & MAKE_CODE), size, byte);
      size = 1;
    }

    tickdelay(micros_to_ticks(DELAY_US));
  }

  enable_kbd_interrupts();

  kbd_print_no_sysinb(count_k);

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t bit_no_kbd, bit_no_t;
  int ipc_status, r;
  message msg;

  uint8_t size=1, byte[2];
  count_k = 0, count_t=0;

  // Subscribe kbc interrupts
  if (kbd_subscribe_int(&bit_no_kbd)!=0) {
    printf("Error subscribing kbc interrupts.\n");
    return 1;
  }

  // Subscribe timer interrupts
  if(timer_subscribe_int(&bit_no_t)!=0) {
      printf("Error subscribing timer interrupts.\n");
      return 1;
  }

  while (scan_code!=ESC_BREAK && count_t/sys_hz()<n) {
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(bit_no_kbd)) { /* subscribed interrupt*/
            timer_int_reset();
            kbd_ih();
          
            if(kbd_scancode_complete(byte, &size)) {
              kbd_print_scancode(!(scan_code & MAKE_CODE), size, byte);
              size = 1;
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_t)) { /* subscribed interrupt*/
            timer_int_handler();
            if( (count_t%sys_hz()) == 0 ) {
              timer_print_elapsed_time();
            }
          }
          break;
      }
    }
  }

  if (kbd_unsubscribe_int()!=0) {
    printf("Error unsubscribing kdb interrupts.\n");
    return 1;
  }

  if(timer_unsubscribe_int()!=0) {
    printf("Error unsubscribing timer interrupts\n");
    return 1;
  }

  kbd_print_no_sysinb(count_k);

  return 0;
}
