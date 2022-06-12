
#ifndef __GAME_H
#define __GAME_H

#include <lcom/lcf.h>
#include "letters.h"

typedef struct {
    int mouse_x;
    int mouse_y;
    bool lmb;
} Mouse;

typedef enum StartMenuEntry{
    DEFAULT,
    SINGLE,
    MULTI,
    RULES,
    EXIT_GAME
} StartMenuEntry;

typedef enum Mode {
    MENU,
    SINGLEPLAYER,
    MULTIPLAYER,
    RULES_CREDITS,
    PAUSE,
    SCOREBOARD,
    EXIT
} Mode;

typedef struct {
    Mode mode;
    bool start;
    bool draw;
    bool drawInput;
    bool canAdvance;
    int sentence;
    bool clean;
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

void changeMenuState();

int singlePlayer_mode(letter **sentence,letter ** sentence2, letter **inputSentence, unsigned int idx);

int pause();

int drawMouse();

int drawPauseMenu();

void nextStartSelected();

void prevStartSelected();

void mouse_handler(struct packet * p);

int kbd_handler(letter * sentence,letter * sentence2, letter ** inputSentence, unsigned int * idx);

int singlePlayer_start(letter **sentence, letter **inputSentence);

int draw_sentence(letter *sentence, uint16_t x, uint16_t y, int correct);

int draw_input(letter *sentence, uint16_t x, uint16_t y, int correct, int size);

int draw_input_sentence(letter *input,letter *sentence, uint16_t x, uint16_t y, int size);

void generateSentence(letter ** sentence, letter ** sentence2);

void allocateSentence(letter ** sentence);

bool isEqual(unsigned int idx,unsigned int number_Letters, letter ** sentence,letter **inputSentence);

//int draw_letter(uint8_t *sprite, uint16_t x, uint16_t y, xpm_image_t img, void * buff, void * color);
 
#endif /* __GAME_H */
