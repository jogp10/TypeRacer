// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "mouse.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (mouse_read_byte)(uint8_t *byte) {
    int ret = 0;
    uint8_t stat;
    for(int i = 0; i < 20; ++i){
        if((ret = util_sys_inb(0X64, &stat))) return ret;
        if((stat&BIT(0)) && (stat&BIT(5))) {
            if(stat & (KBC_PAR_ERR | BIT(6))) return -1;
            if((ret = util_sys_inb(0x60, byte))) return ret;
            else return 0;
        }
        TIME_DELAY;
    }
    return -1;
}
int (kbc_issue_cmd)(uint8_t cmd){
    int ret = 0;
    uint8_t stat;
    for(int i = 0; i < 20; ++i){
        if((ret = util_sys_inb(0x64, &stat))) return ret;
        if((stat&BIT(1)) == 0){
            if(sys_outb(0x64, cmd)) return -1;
            return 0;
        }
        TIME_DELAY;
    }
    return -1;
}

int (kbc_issue_arg)(uint8_t arg){
    int ret = 0;
    uint8_t stat;
    for(int i = 0; i < 20; ++i){
        if((ret = util_sys_inb(0x64, &stat))) return ret;
        if((stat&BIT(1)) == 0){
            if(sys_outb(0x60, arg)) return -1;
            return 0;
        }
        TIME_DELAY;
    }
    return -1;
}
int (mouse_issue_cmd)(uint32_t cmd) {
    int ret;
    uint8_t ack = 0;
    for (unsigned int i = 0; i < 20; i++) {
        if ((ret = kbc_issue_cmd(0xD4))) return ret;
        if ((ret = kbc_issue_arg(cmd))) return ret;
        if ((ret = mouse_read_byte(&ack))) return ret;

        if (ack == 0xFA) return 0;
        if (ack == 0XFC) return -1;
        TIME_DELAY;
    }
    return -1;
}
int (mouse_set_data_report)(int on){
    if(on) return mouse_issue_cmd(0xF4);
    else   return mouse_issue_cmd(0xF5);
}

int (mouse_test_packet)(uint32_t cnt) {
  uint8_t bit_no;
  int ipc_status, r;
  message msg;
  uint8_t packets[3];
  struct packet pp;
  uint8_t size = 1;


  /* Subscribing int */
  if( (r = mouse_subscribe_int(&bit_no)) ) {
    printf("Error subscribing kbc interrupt with: %d.\n", r);
    return 1;
  }
      //if (mouse_set_data_report(true)) return 1; // enables mouse data reporting
  
  //mouse_enable_data_reporting();

  if (mouse_enable_data_rep()!=0) {
    printf("Error enabling mouse data report\n");
    return 1;
  }

  

  while(cnt>0) {
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
            mouse_ih();

            if (mouse_packet_complete(packets, &size)) {
              build_packet_struct(packets, &pp);
              cnt--;
              size = 1;
              mouse_print_packet(&pp);
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
  }

  if (mouse_disable_data_rep()) {
    printf("Error disable mouse data report\n");
    return 1;
  }
   // if (mouse_set_data_report(false)) return 1; // enables mouse data reporting
   
    /* Unsubscribing int */
  if ( (r = mouse_unsubscribe_int()) ) {
    printf("Error unsubscribing kbc interrupt with: %d.\n", r);
    return 1;
  }

    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
