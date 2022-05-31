// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include "vg.h"
#include "vg_macros.h"
#include "kbd.h"
#include "i8042.h"
#include "timer.h"
#include "i8254.h"

#include <stdint.h>
#include <stdio.h>

extern uint8_t code;
extern unsigned int counter_timer, counter_kbd;

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
  // change to graphics mode
  if (vg_change_mode(mode)) {
    vg_exit();
    printf("Error setting graphics mode.\n");
    return 1;
  }

  // delay
  TIME_DELAY(delay);

  // change to default's text mode
  if (vg_exit()) {
    printf("Error returning default's Minix text mode.\n");
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

   /* Subscribing int */
  if( (r = kbc_subscribe_int(&bit_no)) ) {
    vg_exit();
    printf("Error subscribing kbc interrupt with: %d.\n", r);
    return 1;
  }

  // change to graphics mode
  if (vg_change_mode(mode)) {
    vg_exit();
    printf("Error setting graphics mode.\n");
    return 1;
  }

  if (vg_draw_rectangle(x, y, width, height, color)) {
    vg_exit();
    printf("Erro drawing rectangle");
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
  } while (code != ESC_BREAK); // while escape not released

  /* Unsubscribing int */
  if ( (r = kbc_unsubscribe_int()) ) {
    vg_exit();
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }

  // change to default's text mode
  if (vg_exit()) {
    printf("Error returning default's Minix text mode.\n");
    return 1;
  }
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  uint8_t bit_no;
  int ipc_status, r;
  message msg;
  uint8_t scan_code[2], size=1;
  vbe_mode_info_t info;

  // change to graphics mode
  if (vg_change_mode(mode)) {
    printf("Error setting graphics mode.\n");
    return 1;
  }

  vbe_get_info_mode(mode, &info);

  // draw pattern
  uint16_t width = info.XResolution / no_rectangles;
  uint16_t height = info.YResolution / no_rectangles;
  uint32_t color;

  for (int row = 0; row < no_rectangles; row++) {
    for (int col = 0; col < no_rectangles; col++) {

      if (mode == INDEXED_COLOR) {
        color = (first + (row * no_rectangles + col) * step) % (1 << info.BitsPerPixel);
      } else {
        uint8_t red = (R(first) + col * step) % (1 << info.RedMaskSize);
        uint8_t green = (G(first) + row * step) % (1 << info.GreenMaskSize);
        uint8_t blue = (B(first) + (col + row) * step) % (1 << info.BlueMaskSize);
        color = (red << info.RedFieldPosition) | (green << info.GreenFieldPosition) | (blue << info.BlueFieldPosition);
      }

      if (vg_draw_rectangle(col * width, row * height, width, height, color) ) {
        vg_exit();
        printf("Error drawing rectangle.\n");
        return 1;
      }
    }
  }

    /* Subscribing int */
  if( (r = kbc_subscribe_int(&bit_no)) ) {
    vg_exit();
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
  } while (code != ESC_BREAK); // while escape not released

  /* Unsubscribing int */
  if ( (r = kbc_unsubscribe_int()) ) {
    vg_exit();
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }

  // change to default's text mode
  if (vg_exit()) {
    printf("Error returning default's Minix text mode.\n");
    return 1;
  }
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  uint8_t bit_no;
  int ipc_status, r;
  message msg;
  uint8_t scan_code[2], size=1;
  counter_kbd = 0;


  // switch video adapter to graphics mode using VBE
  if(vg_change_mode(MODE1)) {
    printf("Error changing vc mode to %03x.\n", MODE1);
    vg_exit();
    return 1;
  };

  if (vg_draw_xpm(xpm, x, y)) {
    printf("Error drawing xpm.\n");
    return 1;
  }

  /* Subscribing int */
  if( (r = kbc_subscribe_int(&bit_no)) ) {
    vg_exit();
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
  } while (code != ESC_BREAK); // while escape not released


      /* Unsubscribing int */
  if ( (r = kbc_unsubscribe_int()) ) {
    vg_exit();
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


int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  uint8_t bit_no_kbd, bit_no_t;
  int ipc_status, r;
  message msg;
  uint8_t scan_code[2], size=1;
  counter_kbd = 0;
  counter_timer = 0;
  uint16_t x = xi, y = yi;

  // switch video adapter to graphics mode using VBE
  if(vg_change_mode(MODE1)) {
    printf("Error changing vc mode to %03x.\n", MODE1);
    vg_exit();
    return 1;
  };

  // animation
  if (vg_draw_xpm(xpm, x, y)) {
    printf("Error drawing initial xpm position.\n");
    return 1;
  }


  /* Subscribing int */
  if( (r = kbc_subscribe_int(&bit_no_kbd)) ) {
    vg_exit();
    printf("Error subscribing kbc interrupt with: %d.\n", r);
    return 1;
  }
    if( (r = timer_subscribe_int(&bit_no_t)) ) {
    printf("Error subscribing timer interrupt with: %d.\n", r);
    return 1;
  }

  if (timer_set_frequency(0, fr_rate)) {
    printf("Error setting timer frequency to %d.\n", fr_rate);
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
          if (msg.m_notify.interrupts & BIT(bit_no_kbd)) { /* subscribed interrupt */
            /* process it */
            kbc_ih();
            if( kbc_code_complete(scan_code, &size) ) {
              size = 1;
            }
          }
          if (msg.m_notify.interrupts & BIT(bit_no_t)) { /* subscribed timer interrupt */
            /* process it */
            timer_int_handler();
            //(counter_timer%sys_hz()==0); // one second passed
            // draw xpm
            if(speed > 0) {
              vg_move_xpm(xpm, &x, &y, xf, yf, speed);
            } else if (counter_timer%abs(speed)==0) {
              vg_move_xpm(xpm, &x, &y, xf, yf, 1);
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
  } while (code != ESC_BREAK); // while escape not released

  /* Unsubscribing int */
  if ( (r = kbc_unsubscribe_int()) ) {
    vg_exit();
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }
    if ( (r = timer_unsubscribe_int()) ) {
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

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
