#include "game.h"

#include "devices/graphics/vg.h"
#include "devices/kbc/kbd.h"
#include "devices/kbc/mouse.h"
#include <lcom/timer.h>
#include "devices/graphics/vg_macros.h"
#include "devices/kbc/i8042.h"
#include "devices/timer/i8254.h"
#include "devices/utils/utils.h"
#include <lcom/lcf.h>
#include "xpms/xpm.h"

uint8_t timer_bit_no, kb_bit_no, mouse_bit_no;
int selector = 0;
extern unsigned int counter_kbd, counter_timer;
extern uint8_t code;

extern xpm_image_t mouse_img;
extern xpm_image_t menu_start_img;
extern xpm_image_t menu_single_img;
extern xpm_image_t menu_multi_img;
extern xpm_image_t menu_rules_img;
extern xpm_image_t menu_leave_img;
extern uint8_t *mouse_cursor;
extern uint8_t *menu_start;
extern uint8_t *menu_single;
extern uint8_t *menu_multi;
extern uint8_t *menu_rules;
extern uint8_t *menu_leave;

int game_init(Game *self) {
    game = self;

    game->state.mode = MENU;
    game->state.draw = false;
    game->state.start = true;

    int ipc_status, r;
    message msg;
    uint8_t scan_code[2], size_kbd=1;
    counter_kbd = 0;
    counter_timer = 0;
    //uint8_t packets[3];
    //struct packet pp;
    //uint8_t size_mouse = 1;
    //game->mouse.mouse_x=get_hres()/2;
    //game->mouse.mouse_y=get_vres()/2;

    do {
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & BIT(timer_bit_no)) { /* timer interrupt */
                        /* process it */
                        timer_int_handler();

                        printf("timer\n");
                        switch (game->state.mode) {
                            case MENU:
                                if (menu()) return 1;
                                break;
                            default:
                                break;
                        }
                    }
                    if (msg.m_notify.interrupts & BIT(kb_bit_no)) { /* keyboard interrupt */
                        /* process it */
                        kbd_ih();

                        printf("keyboard\n");
                        if( kbd_code_complete(scan_code, &size_kbd) ) {
                            kbd_print_scancode(!(code & MAKE_CODE), size_kbd, scan_code);
                            kbd_handler();
                            size_kbd = 1;
                        }
                    }
                    /*
                    if (msg.m_notify.interrupts & BIT(mouse_bit_no)) {  //mouse interrupt
                        mouse_ih();

                        printf("mouse\n");
                        if (mouse_packet_complete(packets, &size_mouse)) {
                            build_packet_struct(packets, &pp);
                            size_mouse = 1;
                            mouse_print_packet(&pp);
                            mouse_handler(&pp);
                        }

                    }*/
                    break;
                default:
                break; /* no other notifications expected: do nothing */
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    } while (game->state.mode != EXIT);

    return 0;
}

void mouse_handler(struct packet * p){
    game->mouse.lmb = p->lb;
    if(game->mouse.mouse_x + p->delta_x >= 0 && game->mouse.mouse_x + p->delta_x + 15 <= (int) get_hres()){
        game->mouse.mouse_x += p->delta_x;
    }
    if(game->mouse.mouse_y - p->delta_y >= 0 && game->mouse.mouse_y - p->delta_y + 26 <= (int) get_vres()){
        game->mouse.mouse_y -= p->delta_y;
    }
}

int menu() {
    if (game->state.start) {

        game->mouse.lmb = false;
        drawStartMenu();
        game->state.start = false;

    } else {
        drawStartMenu();
    }
    return 0;
}

int(drawStartMenu)(){
    if(vg_draw_rectangle(0, 0, 1152, 864, 0x1F)){
        vg_exit();
        printf("%s: Error drawing rectangle", __func__);
        return 1;
    }
    /* if(vg_draw_xpm(0,0, menu_start_img, menu_start)){
            vg_exit();
            printf("%s: Error drawing xpm", __func__);
            return 1;
        }*/
    switch (selector)
    {
    case 0:
        if(vg_draw_xpm(0,0, menu_single_img, menu_single)){
            vg_exit();
            printf("%s: Error drawing xpm", __func__);
            return 1;
        }
       
        break;
    case 1:
        if(vg_draw_xpm(0,0, menu_multi_img, menu_multi)){
            vg_exit();
            printf("%s: Error drawing xpm", __func__);
            return 1;
        }
        break;
    case 2:
        if(vg_draw_xpm(0,0, menu_rules_img, menu_rules)){
            vg_exit();
            printf("%s: Error drawing xpm", __func__);
            return 1;
        }
        break;
    case 3:
        if(vg_draw_xpm(0,0, menu_leave_img, menu_leave)){
            vg_exit();
            printf("%s: Error drawing xpm", __func__);
            return 1;
        }
        break;
    default:
        break;
    }
    

    // draw rectangle highlights
    //if (game->mouse.mouse_x >= 0 && game->mouse.mouse_x <= (int) get_hres()) {}


    /*if(vg_draw_xpm(game->mouse.mouse_x, game->mouse.mouse_y, mouse_img, mouse_cursor)){
        vg_exit();
        printf("%s: Error drawing xpm", __func__);
        return 1;
    }*/

    double_buffering();
    
    return 0;
}

int drawPauseMenu() {
    /*if(vg_draw_rectangle(0, 0, 1152, 864, 0x1F)){
        vg_exit();
        printf("%s: Error drawing rectangle", __func__);
        return 1;
    }
    
    if(vg_draw_xpm(pause_menu_xpm, (1152-686)/2, (864-570)/2)){
        vg_exit();
        printf("%s: Error drawing xpm", __func__);
        return 1;
    }*/

    // draw rectangle highlights
    //if (game->mouse.mouse_x >= 0 && game->mouse.mouse_x <= (int) get_hres()) {}


    /*if(vg_draw_xpm(game->mouse.mouse_x, game->mouse.mouse_y, mouse_img, mouse_cursor)){
        vg_exit();
        printf("%s: Error drawing xpm", __func__);
        return 1;
    }*/

    double_buffering();
    
    return 0;
}

int kbd_handler(){
    switch (code)
    {
    case 0x4B: //ARROW LEFT
        selector = selector-1;
        if(selector < 0) selector = 3;
        printf("left\n");
        break;
    case 0x4D: //ARROW RIGHT
        printf("right\n");
        selector = (selector+1)%4; 
        break;
    case 0x1C: //ENTER
        printf("enter\n");
        if (selector == 3){
            game->state.mode = EXIT;
        }
        
    default:
        break;
    }
    return 0;
  

  
}


