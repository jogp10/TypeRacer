#include <lcom/lcf.h>

#include <stdint.h>

#include "mouse.h"
#include "kbd.h"
#include "i8042.h"

int hook_id_mouse = 2;
bool error;
uint8_t packet;

int (mouse_subscribe_int)(uint8_t *bit_no) {
    *bit_no = hook_id_mouse;
    if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse)!=0) {
        printf("Error subscribing mouse interruption.\n");
        return 1;
    }
    return 0;
}

int (mouse_unsubscribe_int)() {
    if(sys_irqrmpolicy(&hook_id_mouse)) {
        printf("Error unsubscribing mouse interruption.\n");
        return 1;
    };
    return 0;
}

int (mouse_issue_command)(uint8_t cmd) {
    uint8_t acknowledgment_byte = MC_NACK;
    uint8_t num_tries = 0;

    while(num_tries < MAX_TRIES) 
    {
        if(kbc_issue_command(MC_WRITE)!=0) {
            printf("Error issuing command with arg (kbc)");
            return 1;
        }


        if(kbc_issue_command_with_arg(cmd)!=0) {
            printf("Error issuing command with arg (kbc)");
            return 1;
        }
        if(kbc_read_acknowledgment(&acknowledgment_byte)) {
            printf("Error reading ack byte.");
            return 1;
        }
        

        if(acknowledgment_byte == MC_ACK) {
            return 0;
        }
        if(acknowledgment_byte == MC_ERROR) {
            printf("Ack byte read error.");
            return 1;
        }

        num_tries++;
    }
    
    return 1;
}

int (mouse_set_stream_mode)()  { return mouse_issue_command(MC_SET_STREAM_MODE); }

int (mouse_enable_data_rep)()  { 
    if (mouse_issue_command(MC_EN_DATA_REP)!=0) {
        printf("Error issuing command.");
        return 1;
    } 
    return 0;
}

int (mouse_disable_data_rep)() {
    if (mouse_issue_command(MC_DIS_DATA_REP)) {
        printf("Error issuing command.");
        return 1;
    } 
    return 0;
}

void (mouse_ih)() {
    uint8_t status;
    error = false;

    if (util_sys_inb(KBC_ST_REG, &status)) {
      printf("Error reading status register.\n");
      error = true;
      return;
    }

    if (util_sys_inb(KBC_OUT_BUF, &packet)) { /* read output buffer */
        error = true;
        printf("Error reading output buffer.\n");
        return;
    }

    if(!(status & KBC_AUX)) {
        printf("Not Aux device data");
        return;
    }

    if ( (status & (KBC_PAR_ERR | KBC_TO_ERR))) {
        error = true;
        printf("Error parity or timeout error.\n");
        return;
    } else error = false;
}

bool (mouse_packet_complete)(uint8_t packets[], uint8_t *size) {
    if (error) {
        printf("error\n");
        return false;
    }
    if (*size==1 && !(packet & FIRST_PACKET)) {
        printf("Not first packet");
        return false;
    }

    packets[(*size)-1] = packet;
    if(*size==3) return true;
    (*size)++;
    //printf("not complete\n");
    return false;
}

int (build_packet_struct)(uint8_t packets[], struct packet *pp) {

    for (int i = 0; i<3; i++) {
        pp->bytes[i] = packets[i];
    }

    // Determines buttons pressed
    pp->lb = (packets[0] & LB);
    pp->rb = (packets[0] & RB);
    pp->mb = (packets[0] & MB);

    // Calculates displacement
    pp->x_ov = (packets[0] & X_OVF);
    pp->y_ov = (packets[0] & Y_OVF);

    // Determines if x is negative
    if (packets[0] & X_SIGN) pp->delta_x = packets[1] - 256;
    else pp->delta_x = packets[1];

    // Determines if y is negative
    if (packets[0] & Y_SIGN) pp->delta_y = packets[2] - 256;
    else pp->delta_y = packets[2];

    return 0;
}
