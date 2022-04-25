#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include "kbd.h"
#include "timer.c"

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"

extern unsigned int counter_kbd, counter_timer;
extern uint8_t code;

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
  uint8_t scan_code[2], size=1;
  counter_kbd = 0;

  /* Subscribing int */
  if( (r = kbc_subscribe_int(&bit_no)) ) {
    printf("Error subscribing kbc interrupt with: %d.\n", r);
    return 1;
  }

  do {
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
            /* process it */
            kbc_ih();

            if( kbc_code_complete(scan_code, &size) ) {
              kbd_print_scancode( !(code & MAKE_CODE), size, scan_code);
              size = 1;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
    TIME_DELAY; // e.g. tickdelay()
  } while (code != ESC_BREAK); // while escape not released

  /* Unsubscribing int */
  if ( (r = kbc_unsubscribe_int()) ) {
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }


  if (kbd_print_no_sysinb(counter_kbd)) {
    printf("Error printing number of sys inb calls.\n");
    return 1;
  };

  return 0;
}



int(kbd_test_poll)() {
  uint8_t scan_code[2], size = 1;
  counter_kbd = 0, counter_timer = 0;

  while(code != ESC_BREAK) {
    kbc_ih();

    if( kbc_code_complete(scan_code, &size) ) {
      kbd_print_scancode( !(code & MAKE_CODE), size, scan_code);
      size = 1;
    }

    TIME_DELAY;
  }

  if (kbc_reenable_int()) {
    printf("Error reenabling kbc interrupts.\n");
    return 1;
  }

  if (kbd_print_no_sysinb(counter_kbd)) {
    printf("Error printing number of sys inb calls.\n");
    return 1;
  };

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t bit_no_kbd, bit_no_t;
  int ipc_status, r;
  message msg;
  uint8_t scan_code[2], size=1;
  counter_kbd = 0;

  /* Subscribing int */
  if( (r = kbc_subscribe_int(&bit_no_kbd)) ) {
    printf("Error subscribing kbc interrupt with: %d.\n", r);
    return 1;
  }
  if( (r = timer_subscribe_int(&bit_no_t)) ) {
    printf("Error subscribing timer interrupt with: %d.\n", r);
    return 1;
  }

  do {
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(bit_no_kbd)) { /* subscribed kbd interrupt */
            /* process it */
            counter_timer = 0;
            kbc_ih();

            if( kbc_code_complete(scan_code, &size) ) {
              kbd_print_scancode( !(code & MAKE_CODE), size, scan_code);
              size = 1;
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_t)) { /* subscribed timer interrupt */
            /* process it */
            timer_int_handler();

            if (counter_timer%sys_hz()==0) {
              timer_print_elapsed_time();
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
    TIME_DELAY; // e.g. tickdelay()
  } while (code != ESC_BREAK && counter_timer/sys_hz()<n ); // while escape not released

  /* Unsubscribing int */
  if ( (r = kbc_unsubscribe_int()) ) {
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }
  if ( (r = timer_unsubscribe_int()) ) {
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }


  if (kbd_print_no_sysinb(counter_kbd)) {
    printf("Error printing number of sys inb calls.\n");
    return 1;
  };

  return 0;


  return 1;
}
