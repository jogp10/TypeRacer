// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include "vc.h"
#include "kbd.h"

#include "i8042.h"
#include "vc_macros.h"

#include <stdint.h>
#include <stdio.h>

extern unsigned int counter_kbd;
extern uint8_t code;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

  // switch video adapter to graphics mode using VBE
  if(vc_change_mode(mode)) {
    printf("Error changing vc mode to %03x.\n", mode);
    vg_exit();
    return 1;
  };

  // delay
  tickdelay(micros_to_ticks(TO_SEC(delay)));

  // switch to default text mode
  if(vg_exit()) {
    printf("Error changing vc mode back to text (default mode).\n");
    return 1;
  }
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  uint8_t bit_no;
  int ipc_status, r;
  message msg;
  uint8_t scan_code[2], size=1;
  counter_kbd = 0;

  // switch video adapter to graphics mode using VBE
  if(vc_change_mode(mode)) {
    vg_exit();
    printf("Error changing vc mode to %03x.\n", mode);
    return 1;
  };

  if (vg_draw_rectangle(x, y, width, height, color)) {
    vg_exit();
    printf("Error drawing rectangle.\n");
    return 1;
  };

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

  // switch to default text mode
  if(vg_exit()) {
    printf("Error changing vc mode back to text (default mode).\n");
    return 1;
  }
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  
  uint8_t bit_no;
  int ipc_status, r;
  message msg;
  uint8_t scan_code[2], size=1;
  counter_kbd = 0;

  // switch video adapter to graphics mode using VBE
  if(vc_change_mode(mode)) {
    vg_exit();
    printf("Error changing vc mode to %03x.\n", mode);
    return 1;
  };



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

  // switch to default text mode
  if(vg_exit()) {
    printf("Error changing vc mode back to text (default mode).\n");
    return 1;
  }
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  /* To be completed */
  printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
