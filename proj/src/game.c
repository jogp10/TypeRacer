#include <lcom/lcf.h>
#include "game.h"
#include "devices/graphics/vc.h"

int (vg_init_graphics)(){
    uint16_t mode = 0x115; /* 1024x768 indexed color with 8 bits per pixel */
    if(vc_change_mode(mode) != OK){
        printf("%s: Error initializing graphics mode.", __func__);
        return 1;
    }

    /* Drawing a white rectangle that occupies whole window as a background*/
    if(vg_draw_rectangle(0, 0, 800, 600, 0x1F) != OK){
        printf("%s: Error initializing graphics mode.", __func__);
        return 1;
    }
    return 0;
}
