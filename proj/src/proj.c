#include <lcom/lcf.h>
#include <lcom/proj.h>

#include "devices/graphics/vg.h"
#include "devices/graphics/vg_macros.h"
#include "devices/kbc/kbd.h"
#include "devices/kbc/i8042.h"
#include <lcom/timer.h>
#include "devices/timer/i8254.h"
#include "devices/utils/utils.h"

#include "game.h"
#include "xpms/xpm.h"

#include <unistd.h>

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
  int ipc_status, r;
  message msg;
  uint8_t scan_code[2], size=1;
  counter_kbd = 0;

  /* Subscribing int */
  if( (r = kbd_subscribe_int(&kb_bit_no)) ) {
    printf("Error subscribing kbc interrupt with: %d.\n", r);
    return 1;
  }  

  if( (r = timer_subscribe_int(&timer_bit_no)) ) {
    printf("Error subscribing timer interrupt with: %d.\n", r);
    return 1;
  }  

  if (timer_set_frequency(0, 60)) {
    printf("Error setting timer 0 frequency.\n");
    return 1;
  }

  if (vg_change_mode(0x14c)) {
    vg_exit();
    printf("%s: Error initializing graphics mode.", __func__);
    return 1;
  }

  if(vg_draw_rectangle(0, 0, 1152, 864, 0x1F)){
    vg_exit();
    printf("%s: Error drawing rectangle", __func__);
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
          if (msg.m_notify.interrupts & BIT(kb_bit_no)) { /* subscribed interrupt */
            /* process it */
            kbd_ih();

            if( kbd_code_complete(scan_code, &size) ) {
              kbd_print_scancode( !(code & MAKE_CODE), size, scan_code);
              size = 1;
            }
          }
          if (msg.m_notify.interrupts & BIT(timer_bit_no)) { /* subscribed interrupt */
            /* process it */
            timer_int_handler();
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
  } while (code != ESC_BREAK); // while escape not released

  if(vg_exit() != OK){
    printf("%s: Error exiting graphics mode.", __func__);
    return 1;
  }

  if (timer_unsubscribe_int() != OK){
    printf("Error unsubscribing timer interrupt with: %d.\n", r);
    return 1;
  }

  if (kbd_unsubscribe_int() != OK){
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }

  return 0;
}


