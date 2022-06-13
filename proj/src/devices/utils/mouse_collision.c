#include "mouse_collision.h"

int isInsideOpt(Point opt[], Point mouse){
    //opt[0] - canto superior direito
    //opt[1] - canto inferior esquerdo

    if(mouse.x >= opt[1].x && mouse.x <= opt[0].x && mouse.y <= opt[1].y && mouse.y >= opt[0].y){
        return true;
    }

    return false;
}

