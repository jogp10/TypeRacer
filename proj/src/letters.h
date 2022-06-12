#ifndef __LETTERS_H
#define __LETTERS_H

#include <lcom/lcf.h>

typedef struct letter{
    char letter;         
    uint8_t makeCode;   
    xpm_image_t img;    
    uint8_t * xpm;
    int shift;
}letter;

letter * letters;   

letter ** sentences; 

letter * errorMessage;


//The sentences cant have "."s in the middle of them or else the program dies.
static char sentencesChar[8][500]={
    {"Racism is a part of it, though somewhat muted in recent days - wrote Kirkpatrick Sale in his book."}, 
    {"We still have a couple of big decisions to make, let's focus."}, 
    {"A not-that-rare occurence in Manhattan parks, unfortunately, but, after the fight, it wasn't clear who won."},
    {"On Thursday, Roger Goodell sent a message to all NFL staff members about the domestic abuse issue."},

    {"The companies have promised to do better, including enhancing the nutritional quality of cereals."},
    {"The company is listed on the Tokyo Stock Exchange, it must be very valuable."}, 
    {"The stained glass window had slivers of yellow, orange, blue, white and red."}, 
    {"There's no single checklist, but Tyson offered the following suggestions: a strong understanding of energy units."}
};

int (load)();

void loadSentences();

letter ** allocate2dArray(int row, int col);

void free2dArray(letter ** ptr, int row, int col);

void destroySentencesAndLetters();

#endif /* __LETTERS_H */
