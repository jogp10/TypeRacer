#include "devices/graphics/vc.h"
#include "devices/kbd/kbd.h"
#include <lcom/timer.h>
#include "devices/utils/utils.h"
#include <unistd.h>
#include "devices/kbd/i8042.h"
#include "devices/timer/i8254.h"
#include "game.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/src/logs/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/src/logs/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

uint8_t timer_bit_no, kb_bit_no, mouse_bit_no;
extern unsigned int counter_kbd, counter_timer;
extern uint8_t code;

int(proj_main_loop)(int argc, char* argv[])
{ 

  if(vg_init_graphics() != OK ){
    printf("%s: Error initializing graphics", __func__);
    return 1;
  }

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


  if (kbc_unsubscribe_int() != OK){
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }

  if(vg_exit() != OK){
    printf("%s: Error exiting graphics mode.", __func__);
    return 1;
  }

  return 0;
}


