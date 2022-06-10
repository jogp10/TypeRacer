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
    {"If I had known you were going to come straight up to my office I would not have bothered hunting you in the first place."},
    {"If I had known you were going to come straight up to my office I would not have bothered hunting you in the first place."},
    {"If I had known you were going to come straight up to my office I would not have bothered hunting you in the first place."}
    };

int (load)();

void loadSentences();

letter ** allocateTwoDimenArrayOnHeapUsingMalloc(int row, int col);

void destroyTwoDimenArrayOnHeapUsingFree(letter ** ptr, int row, int col);

void destroySentencesAndLetters();

#endif /* __LETTERS_H */
