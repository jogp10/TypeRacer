
#ifndef __GAME_H
#define __GAME_H

#include <lcom/lcf.h>

typedef struct {
    int mouse_x;
    int mouse_y;
    bool lmb;
} Mouse;

typedef enum Mode {
    MENU,
    MULTIPLAYER,
    SCOREBOARD,
    EXIT
} Mode;

typedef struct {
    Mode mode;
    bool start;
    bool draw;
} State;

typedef struct 
{
    /* data */
    State state;

    Mouse mouse;
} Game;

Game *game;

int game_init(Game *self);

int menu();

int drawStartMenu();

int drawPauseMenu();

void mouse_handler(struct packet * p);

#endif /* __GAME_H */
