#include <lcom/lcf.h>
#include <lcom/keyboard.h>
#include <stdint.h>
#include "i8042.h"

uint8_t scancode = 0x00;
int kb_hook_id = KEYBOARD_IRQ;
bool status_register_error = false;
uint32_t sys_inb_counter = 0;

// Count sys_inb calls, only in lab3
#ifdef LAB3
int sys_inb_count(int port, uint8_t *value) {
	sys_inb_counter++;
	return util_sys_inb(port, value);
}
#else
int sys_inb_count(int port, uint8_t *value) {
	return util_sys_inb(port, value);
}
#endif


int(keyboard_subscribe_int)(uint8_t *bit_no){
    *bit_no = BIT(kb_hook_id);
    if(sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &kb_hook_id) != 0){
        printf("Error subscribing int %s", __func__);
        return 1;
    }
    return 0; 
}

int(keyboard_unsubscribe_int)(){
    if(sys_irqrmpolicy(&kb_hook_id) != 0){
        printf("Error unsubscribing int %s", __func__);
        return 1;
    }
    return 0;
}

void(KBC_IH)(){
    get_scancode();
}

void(get_scancode)(){
    
    uint8_t status, scancode_temp;
    sys_inb_count(KBD_STATUS_REG, &status); //Gets full status register

    //Check if output buffer is full
    if(status & OUT_BUF_FULL){
        sys_inb_count(KBD_OUT_BUF, &scancode_temp);
        //Chech if there are parity or timeout errors
        if(status & (KB_PARITY_ERROR | KB_TIMEOUT_ERROR) == NO_ERROR){
            scancode = scancode_temp;
        }
        else status_register_error = true;
    }
    else status_register_error = true;
    
    status_register_error = false;

}
