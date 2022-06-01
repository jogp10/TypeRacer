
#ifndef __GAME_H
#define __GAME_H

#include <lcom/lcf.h>

typedef struct 
{
    /* data */
    int fullfill;
} Game;

int game_init(Game *self);



#endif /* __GAME_H */
