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
    {"The company is listed on the Tokyo Stock Exchange. It must be very valuable."},
    {"The stained glass window had slivers of yellow, orange, and red. They were pretty"},
    {"We still have a couple of big decisions to make. Let it sink in."}
};

int (load)();

void loadSentences();

letter ** allocateTwoDimenArrayOnHeapUsingMalloc(int row, int col);

void destroyTwoDimenArrayOnHeapUsingFree(letter ** ptr, int row, int col);

void destroySentencesAndLetters();

#endif /* __LETTERS_H */
