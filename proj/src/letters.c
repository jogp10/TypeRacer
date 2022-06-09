#include <lcom/lcf.h>
#include "letters.h"
#include "xpms/xpm.h"

int (load)(){
    letters = (letter*)malloc(57*sizeof(letter));

    letters[0].letter = 'a';
    letters[0].xpm = xpm_load(a, XPM_8_8_8_8, &letters[0].img);
    letters[0].makeCode = 0x1E;

    letters[1].letter = 'b';
    letters[1].xpm = xpm_load(b, XPM_8_8_8_8, &letters[1].img);
    letters[1].makeCode = 0x30;

    letters[2].letter = 'c';
    letters[2].xpm = xpm_load(c, XPM_8_8_8_8, &letters[2].img);
    letters[2].makeCode = 0x2E;

    letters[3].letter = 'd';
    letters[3].xpm = xpm_load(d, XPM_8_8_8_8, &letters[3].img);
    letters[3].makeCode = 0x20;

    letters[4].letter = 'e';
    letters[4].xpm = xpm_load(e, XPM_8_8_8_8, &letters[4].img);
    letters[4].makeCode = 0x12;

    letters[5].letter = 'f';
    letters[5].xpm = xpm_load(f, XPM_8_8_8_8, &letters[5].img);
    letters[5].makeCode = 0x21;

    letters[6].letter = 'g';
    letters[6].xpm = xpm_load(g, XPM_8_8_8_8, &letters[6].img);
    letters[6].makeCode = 0x22;

    letters[7].letter = 'h';
    letters[7].xpm = xpm_load(h, XPM_8_8_8_8, &letters[7].img);
    letters[7].makeCode = 0x23;

    letters[8].letter = 'i';
    letters[8].xpm = xpm_load(i,XPM_8_8_8_8,&letters[8].img);
    letters[8].makeCode = 0x17;

    letters[9].letter = 'j';
    letters[9].xpm = xpm_load(j,XPM_8_8_8_8,&letters[9].img);
    letters[9].makeCode = 0x24;

    
    letters[10].letter = 'k';
    letters[10].xpm = xpm_load(k,XPM_8_8_8_8,&letters[10].img);
    letters[10].makeCode = 0x25;


    letters[11].letter = 'l';
    letters[11].xpm = xpm_load(l,XPM_8_8_8_8,&letters[11].img);
    letters[11].makeCode = 0x26;


    letters[12].letter = 'm';
    letters[12].xpm = xpm_load(m,XPM_8_8_8_8,&letters[12].img);
    letters[12].makeCode = 0x32;

    letters[13].letter = 'n';
    letters[13].xpm = xpm_load(n,XPM_8_8_8_8,&letters[13].img);
    letters[13].makeCode = 0x31;


    letters[14].letter = 'o';
    letters[14].xpm = xpm_load(o,XPM_8_8_8_8,&letters[14].img);
    letters[14].makeCode = 0x18;

    letters[15].letter = 'p';
    letters[15].xpm = xpm_load(p,XPM_8_8_8_8,&letters[15].img);
    letters[15].makeCode = 0x19;

    letters[16].letter = 'q';
    letters[16].xpm = xpm_load(q,XPM_8_8_8_8,&letters[16].img);
    letters[16].makeCode = 0x10;

    letters[17].letter = 'r';
    letters[17].xpm = xpm_load(r,XPM_8_8_8_8,&letters[17].img);
    letters[17].makeCode = 0x13;

    letters[18].letter = 's';
    letters[18].xpm = xpm_load(s,XPM_8_8_8_8,&letters[18].img);
    letters[18].makeCode = 0x1F;

    letters[19].letter = 't';
    letters[19].xpm = xpm_load(t,XPM_8_8_8_8,&letters[19].img);
    letters[19].makeCode = 0x14;

    letters[20].letter = 'u';
    letters[20].xpm = xpm_load(u,XPM_8_8_8_8,&letters[20].img);
    letters[20].makeCode = 0x16;

    letters[21].letter = 'v';
    letters[21].xpm = xpm_load(v,XPM_8_8_8_8,&letters[21].img);
    letters[21].makeCode = 0x2F;

    letters[22].letter = 'w';
    letters[22].xpm = xpm_load(w,XPM_8_8_8_8,&letters[22].img);
    letters[22].makeCode = 0x11;

    letters[23].letter = 'x';
    letters[23].xpm = xpm_load(x,XPM_8_8_8_8,&letters[23].img);
    letters[23].makeCode = 0x2D;

    letters[24].letter = 'y';
    letters[24].xpm = xpm_load(y,XPM_8_8_8_8,&letters[24].img);
    letters[24].makeCode = 0x15;

    letters[25].letter = 'z';
    letters[25].xpm = xpm_load(z,XPM_8_8_8_8,&letters[25].img);
    letters[25].makeCode = 0x2C;
    
    letters[26].letter = '.';
    letters[26].xpm = xpm_load(ponto,XPM_8_8_8_8,&letters[26].img);
    letters[26].makeCode = 0x34;

    letters[27].letter = ',';
    letters[27].xpm = xpm_load(virgula,XPM_8_8_8_8,&letters[27].img);
    letters[57].makeCode = 0x33;

    letters[28].letter = ':';
    letters[28].xpm = xpm_load(doisPontos,XPM_8_8_8_8,&letters[28].img);
    letters[28].makeCode = 0x34;

    letters[29].letter = ';';
    letters[29].xpm = xpm_load(pontoeVirgula,XPM_8_8_8_8,&letters[29].img);
    letters[29].makeCode = 0x33;


    letters[30].letter = ' ';
    letters[30].xpm = xpm_load(space,XPM_8_8_8_8,&letters[30].img);
    letters[30].makeCode = 0x44;

  	letters[31].letter = 'A';
    letters[31].xpm = xpm_load(A,XPM_8_8_8_8,&letters[31].img);
    letters[31].makeCode = 0x1E;

    letters[32].letter = 'B';
    letters[32].xpm = xpm_load(B_upper,XPM_8_8_8_8,&letters[32].img);
    letters[32].makeCode = 0x30;
    
    letters[33].letter = 'C';
    letters[33].xpm = xpm_load(C,XPM_8_8_8_8,&letters[33].img);
    letters[33].makeCode = 0x2E;
    
    letters[34].letter = 'D';
    letters[34].xpm = xpm_load(D,XPM_8_8_8_8,&letters[34].img);
    letters[34].makeCode = 0x20;

    letters[35].letter = 'E';
    letters[35].xpm = xpm_load(E,XPM_8_8_8_8,&letters[35].img);
    letters[35].makeCode = 0x12;

    letters[36].letter = 'F';
    letters[36].xpm = xpm_load(F,XPM_8_8_8_8,&letters[36].img);
    letters[36].makeCode = 0x21;

    letters[37].letter = 'G';
    letters[37].xpm = xpm_load(G_upper,XPM_8_8_8_8,&letters[37].img);
    letters[37].makeCode = 0x22;

    letters[38].letter = 'H';
    letters[38].xpm = xpm_load(H,XPM_8_8_8_8,&letters[38].img);
    letters[38].makeCode = 0x23;

    letters[39].letter = 'I';
    letters[39].xpm = xpm_load(I,XPM_8_8_8_8,&letters[39].img);
    letters[39].makeCode = 0x17;

    letters[40].letter = 'J';
    letters[40].xpm = xpm_load(J,XPM_8_8_8_8,&letters[40].img);
    letters[40].makeCode = 0x24;
    
    letters[41].letter = 'K';
    letters[41].xpm = xpm_load(K,XPM_8_8_8_8,&letters[41].img);
    letters[41].makeCode = 0x25;

    letters[42].letter = 'L';
    letters[42].xpm = xpm_load(L,XPM_8_8_8_8,&letters[42].img);
    letters[42].makeCode = 0x26;

    letters[43].letter = 'M';
    letters[43].xpm = xpm_load(M,XPM_8_8_8_8,&letters[43].img);
    letters[43].makeCode = 0x32;

    letters[44].letter = 'N';
    letters[44].xpm = xpm_load(N,XPM_8_8_8_8,&letters[44].img);
    letters[44].makeCode = 0x31;

    letters[45].letter = 'O';
    letters[45].xpm = xpm_load(O,XPM_8_8_8_8,&letters[45].img);
    letters[45].makeCode = 0x18;

    letters[46].letter = 'P';
    letters[46].xpm = xpm_load(P,XPM_8_8_8_8,&letters[46].img);
    letters[46].makeCode = 0x19;

    letters[47].letter = 'Q';
    letters[47].xpm = xpm_load(Q,XPM_8_8_8_8,&letters[47].img);
    letters[47].makeCode = 0x10;

    letters[48].letter = 'R';
    letters[48].xpm = xpm_load(R_upper,XPM_8_8_8_8,&letters[48].img);
    letters[48].makeCode = 0x13;

    letters[49].letter = 'S';
    letters[49].xpm = xpm_load(S,XPM_8_8_8_8,&letters[49].img);
    letters[49].makeCode = 0x1F;

    letters[50].letter = 'T';
    letters[50].xpm = xpm_load(T,XPM_8_8_8_8,&letters[50].img);
    letters[50].makeCode = 0x14;

    letters[51].letter = 'U';
    letters[51].xpm = xpm_load(U,XPM_8_8_8_8,&letters[51].img);
    letters[51].makeCode = 0x16;

    letters[52].letter = 'V';
    letters[52].xpm = xpm_load(V,XPM_8_8_8_8,&letters[52].img);
    letters[52].makeCode = 0x2F;

    letters[53].letter = 'W';
    letters[53].xpm = xpm_load(W,XPM_8_8_8_8,&letters[53].img);
    letters[53].makeCode = 0x11;

    letters[54].letter = 'X';
    letters[54].xpm = xpm_load(X,XPM_8_8_8_8,&letters[54].img);
    letters[54].makeCode = 0x2D;

    letters[55].letter = 'Y';
    letters[55].xpm = xpm_load(Y,XPM_8_8_8_8,&letters[55].img);
    letters[55].makeCode = 0x15;

    letters[56].letter = 'Z';
    letters[56].xpm = xpm_load(Z,XPM_8_8_8_8,&letters[56].img);
    letters[56].makeCode = 0x2C;


    return 0;
}

void loadSentences(){     
  sentences = allocateTwoDimenArrayOnHeapUsingMalloc(3,163);
  for(unsigned i = 0; i<3; i++){ 
    for(unsigned int j=0; j<strlen(sentencesChar[i]); j++){
      for(unsigned int k =0; k < 57; k++){
        if(letters[k].letter == sentencesChar[i][j]){
          sentences[i][j] = letters[k];
          break;
        }
      }
    }
  }


                                                                     
}

letter ** allocateTwoDimenArrayOnHeapUsingMalloc(int row, int col){
	letter ** ptr = (letter**) malloc(sizeof(letter *)*row);
	for(int i = 0; i < row; i++)
	{
		ptr[i] = (letter *) malloc(sizeof(letter)*col);
	}
	return ptr;
}

void destroyTwoDimenArrayOnHeapUsingFree(letter ** ptr, int row, int col){
  for(int i = 0; i < row; i++){
    free(ptr[i]);
  }
  free(ptr);
}

void destroySentencesAndLetters(){
  destroyTwoDimenArrayOnHeapUsingFree(sentences,163,3);
  free(letters);
}
