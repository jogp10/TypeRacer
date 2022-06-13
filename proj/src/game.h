#ifndef __GAME_H
#define __GAME_H

/** @defgroup game game
 * @{
 *
 * game module
 */

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
    EXIT,
    WIN
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

/**
 * @brief Game initiaon
 * 
 * @param self object Game
 * @return Return 0 upon success and non-zero otherwise
 */
int game_init(Game *self);

/**
 * @brief Game menu
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int menu();

/**
 * @brief change menu state
 * 
 */
void changeMenuState();

/**
 * @brief change to rules menu
 * 
 */
void rules();

/**
 * @brief Draws rules menu
 * 
 * @return int 
 */
int drawRulesMenu();

/**
 * @brief change game state to single player
 * 
 * @param sentence first sentece to be written
 * @param sentence2 second sentence to be written
 * @param inputSentence array with input from user
 * @param idx size of input sentence
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int singlePlayer_mode(letter **sentence,letter ** sentence2, letter **inputSentence, unsigned int idx);

/**
 * @brief Pause Menu
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int pause();

/**
 * @brief Winners menu
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int win_menu_f();

/**
 * @brief draw mouse
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int drawMouse();

/**
 * @brief draw Pause Menu
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int drawPauseMenu();

/**
 * @brief highlight menu option
 * 
 */
void nextStartSelected();

/**
 * @brief highlight menu option
 * 
 */
void prevStartSelected();

/**
 * @brief mouse interrupt handler
 * 
 * @param p mouse action
 */
void mouse_handler(struct packet * p);

/**
 * @brief keyboard interrupt handle
 * 
 * @param sentence first sentence to be written
 * @param sentence2 second sentence to be written
 * @param inputSentence array with user input sentence
 * @param idx size of input sentence
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_handler(letter * sentence,letter * sentence2, letter ** inputSentence, unsigned int * idx);

/**
 * @brief starts single player mode
 * 
 * @param sentence first sentence to be written
 * @param inputSentence second sentence to be written
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int singlePlayer_start(letter **sentence, letter **inputSentence);

/**
 * @brief 
 * 
 * @param sentence sentence to be drawn
 * @param x starting position in x axys
 * @param y starting position in y axys
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int draw_sentence(letter *sentence, uint16_t x, uint16_t y);

/**
 * @brief draw input sentence
 * 
 * @param input input sentence
 * @param sentence sentence that is on play
 * @param x starting position in x axys
 * @param y starting position in y axys
 * @param size size of input
 * 
 * @return Return 0 upon success and non-zero otherwise 
 */
int draw_input_sentence(letter *input,letter *sentence, uint16_t x, uint16_t y, int size);

/**
 * @brief draw generated sentence
 * 
 * @param sentence first sentence to be drawn
 * @param sentence2 second sentence to be drawn
 */
void generateSentence(letter ** sentence, letter ** sentence2);

/**
 * @brief allocate memory to sentence
 * 
 * @param sentence pointer to allocate memory on
 */
void allocateSentence(letter ** sentence);

/**
 * @brief check for sentences match
 * 
 * @param idx size of input sentence
 * @param number_Letters size of sentence that is being played
 * @param sentence sentence that is being played
 * @param inputSentence input sentence
 * 
 * @return true on match, false otherwise
 */
bool isEqual(unsigned int idx,unsigned int number_Letters, letter ** sentence,letter **inputSentence);

//int draw_letter(uint8_t *sprite, uint16_t x, uint16_t y, xpm_image_t img, void * buff, void * color);

/**@}*/
 
#endif /* __GAME_H */
