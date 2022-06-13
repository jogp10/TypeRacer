#include <lcom/lcf.h>
#include <lcom/proj.h>
#include "devices/graphics/vg.h"
#include "devices/kbc/kbd.h"
#include "devices/kbc/mouse.h"
#include <lcom/timer.h>
#include "devices/graphics/vg_macros.h"
#include "devices/kbc/i8042.h"
#include "devices/timer/i8254.h"
#include "devices/utils/utils.h"
#include "game.h"
#include "xpms/xpm.h"
extern char *double_buf;
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

extern uint8_t timer_bit_no, kb_bit_no, mouse_bit_no;
extern unsigned int counter_kbd, counter_timer;
extern uint8_t code;

int(proj_main_loop)(int argc, char* argv[])
{ 
  //window size : 1152 x 864

  int r;

  /* Subscribing int */
  if( (r = kbd_subscribe_int(&kb_bit_no)) ) {
    printf("Error subscribing kbc interrupt with: %d.\n", r);
    return 1;
  }  

  if( (r = timer_subscribe_int(&timer_bit_no)) ) {
    printf("Error subscribing timer interrupt with: %d.\n", r);
    return 1;
  }  

  if( (r = mouse_subscribe_int(&mouse_bit_no)) ) {
    printf("Error subscribing mouse interrupt with: %d.\n", r);
    return 1;
  }

  if (vg_change_mode(0x14C)) {
    printf("%s: Error initializing graphics mode.", __func__);
    return 1;
  }

  if(load_all_xpms() == 1){
    if (mouse_unsubscribe_int()){
    printf("Error unsubscribing mouse interrupt with: %d.\n", r);
    return 1;
  }

    if (timer_unsubscribe_int()){
      printf("Error unsubscribing timer interrupt with: %d.\n", r);
      return 1;
    }

    if (kbd_unsubscribe_int()){
      printf("Error unsubscribing kbc interrupt with: %d.\n", r);
      return 1;
    }

    if(vg_exit()){
      printf("%s: Error exiting graphics mode.", __func__);
      return 1;
    }
  }


  Game game;
  game_init(&game);
  

  if (mouse_unsubscribe_int()){
    printf("Error unsubscribing mouse interrupt with: %d.\n", r);
    return 1;
  }

  if (timer_unsubscribe_int()){
    printf("Error unsubscribing timer interrupt with: %d.\n", r);
    return 1;
  }

  if (kbd_unsubscribe_int()){
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }

  if(vg_exit()){
    printf("%s: Error exiting graphics mode.", __func__);
    return 1;
  }
  free(double_buf);
  return 0;
}


