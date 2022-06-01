#include "game.h"

#include "devices/graphics/vg.h"
#include "devices/kbc/kbd.h"
#include "devices/kbc/mouse.h"
#include <lcom/timer.h>
#include "devices/graphics/vg_macros.h"
#include "devices/kbc/i8042.h"
#include "devices/timer/i8254.h"
#include "devices/utils/utils.h"

#include "xpms/xpm.h"

uint8_t timer_bit_no, kb_bit_no, mouse_bit_no;
extern unsigned int counter_kbd, counter_timer;
extern uint8_t code;

int game_init(Game *self) {
    int ipc_status, r;
    message msg;
    uint8_t scan_code[2], size_kbd=1;
    counter_kbd = 0;
    counter_timer = 0;
    uint8_t packets[3];
    struct packet pp;
    uint8_t size_mouse = 1;
    game->mouse.mouse_X=get_hres()/2;
    game->mouse.mouse_Y=get_vres()/2;

    if(vg_draw_rectangle(0, 0, 1152, 864, 0x1F)){
        vg_exit();
        printf("%s: Error drawing rectangle", __func__);
        return 1;
    }

    if(vg_draw_xpm(menu_xpm, (1152-686)/2, (864-570)/2)){
        vg_exit();
        printf("%s: Error drawing xpm", __func__);
        return 1;
    }
    if(vg_draw_xpm(mouse_cursor,mouse->mouse_X, mouse->mouse_Y)){
        vg_exit();
        printf("%s: Error drawing xpm", __func__);
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

                if( kbd_code_complete(scan_code, &size_kbd) ) {
                kbd_print_scancode( !(code & MAKE_CODE), size_kbd, scan_code);
                size_kbd = 1;
                }
            }
            if (msg.m_notify.interrupts & BIT(timer_bit_no)) { /* subscribed interrupt */
                /* process it */
                timer_int_handler();
            }
            if (msg.m_notify.interrupts & BIT(mouse_bit_no)) { /* subscribed interrupt */
                mouse_ih();

                if (mouse_packet_complete(packets, &size_mouse)) {
                build_packet_struct(packets, &pp);
                size_mouse = 1;
                mouse_print_packet(&pp);
                mouse_handler(&pp);
                }

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


    return 0;

}
void mouse_handler(struct packet * p){
    game->mouse.lbPressed = p->lb;
    if(game->mouse.mouse_X + p->delta_x >= 0 && game->mouse.mouse_X + p->delta_x + 15 <= (int) get_hres()){
        game->mouse.mouse_X += p->delta_x;
    }
    if(game->mouse.mouseY-p->delta_y >= 0 && game->mouse.mouse_Y - p->delta_y + 26 <= (int) get_vres()){
        game->mouse.mouse_Y -= p->delta_y;
    }
}
