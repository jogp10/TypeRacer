#ifndef __LETTERS_H
#define __LETTERS_H

#include <lcom/lcf.h>

typedef struct letter{
    char letter;         
    uint8_t makeCode;   
    xpm_image_t img;    
    uint8_t * xpm;  
}letter;

letter * letters;   

letter ** sentences; 

letter * errorMessage;

static char sentencesChar[3][163]={
    {"if i had known dsdsad."},
    {"i am not a religious man, but every now and then, a child come."},
    {"the general is weak and without authority when his orders are not."}
    };

int (load)();

void loadSentences();

letter ** allocateTwoDimenArrayOnHeapUsingMalloc(int row, int col);

void destroyTwoDimenArrayOnHeapUsingFree(letter ** ptr, int row, int col);

void destroySentencesAndLetters();

#endif /* __LETTERS_H */
