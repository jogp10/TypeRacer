#include "game.h"

#include "devices/graphics/vg.h"
#include "devices/kbc/kbd.h"
#include "devices/kbc/mouse.h"
#include <lcom/timer.h>
#include "devices/graphics/vg_macros.h"
#include "devices/kbc/i8042.h"
#include "devices/timer/i8254.h"
#include "devices/utils/utils.h"
#include <lcom/lcf.h>
#include "xpms/xpm.h"
#include "letters.h"
#include "devices/utils/mouse_collision.h"

uint8_t timer_bit_no, kb_bit_no, mouse_bit_no;
int selectorMenu = 0, selectorPause = 0, selectorWin = 0;
unsigned int number_Letters1 = 0,number_Letters2= 0;
static StartMenuEntry startMenuEntry;
static StartMenuEntry prevStartMenuEntry;
static int isUpper = 0, isShift = 0;

extern unsigned int counter_kbd, counter_timer;
extern uint8_t code;

int game_time;

extern letter *letters;
extern xpm_image_t mouse_img;
extern xpm_image_t menu_start_img;
extern xpm_image_t menu_single_img;
extern xpm_image_t menu_multi_img;
extern xpm_image_t menu_rules_img;
extern xpm_image_t menu_leave_img;
extern xpm_image_t menu_pause_img;
extern xpm_image_t menu_pause_resume_img;
extern xpm_image_t menu_pause_exit_img;
extern xpm_image_t game_background_img;
extern xpm_image_t red_car_img;
extern xpm_image_t next_img;
extern xpm_image_t upper_img;
extern xpm_image_t win_menu_img;
extern xpm_image_t win_exit_img;
extern xpm_image_t regras_img;
extern uint8_t *mouse_cursor;
extern uint8_t *menu_start;
extern uint8_t *menu_single;
extern uint8_t *menu_multi;
extern uint8_t *menu_rules;
extern uint8_t *menu_leave;
extern uint8_t *menu_pause;
extern uint8_t *menu_pause_resume;
extern uint8_t *menu_pause_exit;
extern uint8_t *game_background;
extern uint8_t *red_car;
extern uint8_t *next;
extern uint8_t *upper;
extern uint8_t *win_menu;
extern uint8_t *win_exit;
extern uint8_t *regras;


//canto sup dir, canto inf esq
Point singlePlayerOpt[] = {{516,340}, {420,440}};
Point multiPlayerOpt[] = {{660, 310}, {546, 416}};
Point rulesOpt[] = {{842, 270}, {708, 388}};
Point leaveOpt[] = {{1048, 222}, {892, 356}};

xpm_image_t current_menu_img;
uint8_t *current_menu;

int game_init(Game *self) {
    unsigned int idx=0;
    letter *sentence= NULL;
    letter *sentence2= NULL;
    letter *inputSentence = NULL;


    current_menu_img = menu_start_img;
    current_menu = menu_start;
    startMenuEntry = DEFAULT;
    prevStartMenuEntry = DEFAULT;

    load(); 
    loadSentences();
    generateSentence(&sentence,&sentence2);
    
    inputSentence = (letter*)malloc(500*sizeof(letter));  
    game = self;
    game->state.mode = MENU;
    game->state.draw = false;
    game->state.start = false;

    int ipc_status, r;
    message msg;
    uint8_t scan_code[2], size_kbd=1;
    counter_kbd = 0;
    counter_timer = 0;
    uint8_t packets[3];
    struct packet pp;
    uint8_t size_mouse = 1;
    game->mouse.mouse_x=get_hres()/2;
    game->mouse.mouse_y=get_hres()/2;
    game->mouse.lmb= false;
    game->state.sentence = 1;
    menu();
    do {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & BIT(timer_bit_no)) { /* timer interrupt */
                        /* process it */
                        timer_int_handler();             
                        switch (game->state.mode){
                            case MENU:
                                changeMenuState();
                                break;
                            case SINGLEPLAYER:
                                if(singlePlayer_mode(&sentence,&sentence2, &inputSentence,idx)) return 1;
                                break;
                            case MULTIPLAYER:
                                //Under construction
                                break;
                            case RULES_CREDITS:
                                rules();
                                break;
                            case PAUSE:
                                if(pause()) return 1;
                            case SCOREBOARD:
                                //Under construction
                                break;
                            case EXIT:
                                break;
                            case WIN:
                                if(win_menu_f()) return 1;
                                break;
                        }
                        drawMouse();
                    }
                    if (msg.m_notify.interrupts & BIT(mouse_bit_no)) {  //mouse interrupt
                        mouse_ih();
                        if (mouse_packet_complete(packets, &size_mouse)) {
                            build_packet_struct(packets, &pp);
                            size_mouse = 1;
                            mouse_handler(&pp);
                        }
                    }
                    if (msg.m_notify.interrupts & BIT(kb_bit_no)){ /* keyboard interrupt */
                        /* process it */
                        kbd_ih();                      
                        if( kbd_code_complete(scan_code, &size_kbd) ) {
                            kbd_print_scancode(!(code & MAKE_CODE), size_kbd, scan_code);
                            kbd_handler(sentence,sentence2,&inputSentence,&idx);
                            size_kbd = 1;
                        }
                    }
                    break;
                default:
                break; /* no other notifications expected: do nothing */
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    } while (game->state.mode != EXIT);
    free(inputSentence);
    free(sentence2);
    free(sentence);
    free(letters);
    return 0;
}

void changeMenuState(){

    if(!game->state.draw)return;
    switch (selectorMenu){
        case DEFAULT:
            current_menu_img = menu_start_img;
            current_menu = menu_start;

            break;
        case SINGLE:
            current_menu_img = menu_single_img;
            current_menu = menu_single;

            break;
        case MULTI:
            current_menu_img = menu_multi_img;
            current_menu = menu_multi;

            break;  
        case RULES:
            current_menu_img = menu_rules_img;
            current_menu = menu_rules;

            break;
        case EXIT_GAME:
            current_menu_img = menu_leave_img;
            current_menu = menu_leave;

            break;
    }
    prevStartMenuEntry = startMenuEntry;
    vg_draw_xpm(0,0, current_menu_img, current_menu);
    game->state.draw = false;
    double_buffering();

}

void rules(){
    if(game->state.start){
        game->state.draw = true;
        drawRulesMenu();
        game->state.start = false;
    }
}

int drawRulesMenu(){

    if(game->state.draw){
        if(vg_draw_xpm(0,0, regras_img, regras)){
            vg_exit();
            return 1;
        }
        game->state.draw = false;
        
    }
    double_buffering();
    return 0;
}

void mouse_handler(struct packet * p){
    game->mouse.lmb = p->lb;
    clear_xpm_with_cover(game->mouse.mouse_x,game->mouse.mouse_y, 15,26,current_menu_img, current_menu);

    if(game->mouse.mouse_x + p->delta_x >= 0 && game->mouse.mouse_x + p->delta_x + 15 <= (int) get_hres()){
        game->mouse.mouse_x += p->delta_x;
    }
    if(game->mouse.mouse_y - p->delta_y >= 0 && game->mouse.mouse_y - p->delta_y + 26 <= (int) get_vres()){
        game->mouse.mouse_y -= p->delta_y;
    }
    
    Point mouse_pt = {game->mouse.mouse_x,game->mouse.mouse_y};
    if(isInsideOpt(singlePlayerOpt, mouse_pt)){
        
        if(selectorMenu!=1){
            selectorMenu = 1;
            startMenuEntry = SINGLE;
            game->state.draw = true;

        }
        if(game->mouse.lmb){
            game->state.mode = SINGLEPLAYER;
            game->state.start = true;
            current_menu = game_background;
            current_menu_img = game_background_img;
        }
        
    }
    else if(isInsideOpt(multiPlayerOpt, mouse_pt)){
        
        if(selectorMenu!=2){
            selectorMenu = 2;
            startMenuEntry = MULTI;
            game->state.draw = true;
        }
        
    }
    else if(isInsideOpt(rulesOpt, mouse_pt)){
        
        if(selectorMenu!=3){
            selectorMenu = 3;
            startMenuEntry = RULES;
            game->state.draw = true;
        }
        if(game->mouse.lmb){
            game->state.mode = RULES_CREDITS;
            game->state.start = true;
            current_menu = regras;
            current_menu_img = regras_img;
        }
        
    }
    else if(isInsideOpt(leaveOpt, mouse_pt)){

        if(selectorMenu!=4){
            selectorMenu = 4;
            startMenuEntry = EXIT_GAME;
            game->state.draw = true;
        }
        if(game->mouse.lmb){
            game->state.mode=EXIT;
        }
    }
}

int drawMouse(){  
    if(vg_draw_xpm(game->mouse.mouse_x, game->mouse.mouse_y, mouse_img, mouse_cursor)){
        vg_exit();
        return 1;
    }
    double_buffering();
    return 0;
}

int menu() {
    game->mouse.lmb = false;

    if(vg_draw_xpm(0,0, menu_start_img, menu_start)){
        vg_exit();
        return 1;
    }

    double_buffering();
    
    return 0;
}

void nextStartSelected(){  
    
    prevStartMenuEntry = startMenuEntry;
    switch (startMenuEntry)
    {
    case DEFAULT:
        startMenuEntry = SINGLE;
        selectorMenu = 1;
        return;
    case SINGLE:
        startMenuEntry = MULTI;
        selectorMenu = 2;
        return;
    case MULTI:
        startMenuEntry = RULES;
         selectorMenu = 3;
        return;
    case RULES:
        startMenuEntry = EXIT_GAME;
         selectorMenu = 4;
        return;
    case EXIT_GAME:
        startMenuEntry = SINGLE;
         selectorMenu = 1;
        return;
    }
}

void prevStartSelected(){

    switch (startMenuEntry)
    {
    case DEFAULT:
        startMenuEntry = EXIT_GAME;
        selectorMenu = 4;

        return;
    case SINGLE:
        startMenuEntry = EXIT_GAME;
        selectorMenu = 4;
        return;
    case MULTI:
        startMenuEntry = SINGLE;
        selectorMenu = 1;
        return;
    case RULES:
        startMenuEntry = MULTI;
                selectorMenu = 2;
        return;
    case EXIT_GAME:
        startMenuEntry = RULES;
        selectorMenu = 3;
        return;
    }
    game->state.draw = true;

}

int kbd_handler(letter * sentence,letter * sentence2 , letter ** inputSentence, unsigned int * idx){
  
    switch (game->state.mode){
        case MENU:
            switch(code){
                //ARROW LEFT
                case 0x4B:
                    prevStartSelected();
                    game->state.draw = true;
                    break;
                //ARROW RIGHT
                case 0x4D:
                    nextStartSelected();
                    game->state.draw = true;
                    return 0;
                //ENTER
                case 0x1C:
                    switch (startMenuEntry){      
                        case DEFAULT:
                            return 0;
                        case SINGLE:
                            game->state.mode = SINGLEPLAYER;
                            game->state.start = true;
                            current_menu = game_background;
                            current_menu_img = game_background_img;
                            return 0;
                        case MULTI:
                            return 0;
                        case RULES:
                            game->state.mode = RULES_CREDITS;
                            game->state.start = true;
                            current_menu = regras;
                            current_menu_img = regras_img;
                            return 0;
                        case EXIT_GAME:
                            game->state.mode = EXIT;
                            return 0;
                    }
                    return 0;
            }  
        return 0;
        case SINGLEPLAYER:
            switch(code){
                case ESC_BREAK:
                    game->state.mode = PAUSE;
                    game->state.start = true;
                    current_menu = menu_pause_resume;
                    current_menu_img = menu_pause_resume_img;
                    return 0;
                case 0x1C:
                    if(game->state.canAdvance){
                        if(game->state.sentence == 2){
                            game->state.mode = WIN;
                            game->state.start = true;
                            game->state.canAdvance = false;
                            game->state.sentence = 1;
                            (*idx) = 0;
                        }
                        else{
                            game->state.sentence = 2;
                            game->state.start = true;
                            game->state.canAdvance = false;
                            (*idx) = 0;
                        }
                        
                        
                    }
                    return 0;
                case(0x0E):
                    if((*idx)<=0) (*idx) = 0;
                    if((*idx)>0)(*idx)--;
                    game->state.drawInput = true;
                    return 0;
                case(LSHIFT_MAKE):
                    if(isShift == 1)
                        break;
                    if(isUpper == 0){
                        isUpper = 1;
                        isShift = 1;
                    }
                    else{
                        isUpper = 0;
                        isShift = 1;
                        game->state.clean = true;
                    }
                    break;
                case(LSHIFT_BRAKE):
                    if(isUpper == 0){
                        isUpper = 1;
                    }
                    else{
                        isUpper = 0;
                        game->state.clean = true;
                    }
                    isShift = 0;
                    break;
                default:
                    if(isUpper == 0){
                        for(unsigned int k =0; k < 35; k++){ 
                            if(letters[k].makeCode == code && letters[k].shift == isShift){ 
                                

                                (*inputSentence)[*idx] = letters[k];
                                (*idx)++;
                                game->state.drawInput = true;                                
                                return 0;
                            }
                        }
                        return 0;
                    }
                    else{
                        for(unsigned int k =26; k < 61; k++){ 
                            if(letters[k].makeCode == code && letters[k].shift == isShift){ 
                                
                                (*inputSentence)[*idx] = letters[k];
                                (*idx)++;
                                game->state.drawInput = true;                                
                                return 0;
                            }
                        }
                        return 0;
                    }
            }  
            return 0; 
        case MULTIPLAYER:
            return 0;
        case RULES_CREDITS:
            switch (code)
            {
            case ESC_BREAK:
                game->state.mode = MENU;
                game->state.start = true;
                current_menu = menu_start;
                current_menu_img = menu_start_img;
                game->state.draw = true;
                break;
            default:
                break;
            }
            return 0;
        case PAUSE:
            switch(code){
                //ESC
                case ESC_BREAK:
                    game->state.mode = SINGLEPLAYER;
                    game->state.start = true;
                    current_menu = game_background;
                    current_menu_img = game_background_img;
                    return 0;
                //ARROW DOWN
                case 0x48:
                    selectorPause = selectorPause-1;
                    if(selectorPause < 0) selectorPause = 1;
                    game->state.start = true;
                    return 0;
                //ARROW UP
                case 0x50:
                    selectorPause = (selectorPause+1)%2; 
                    game->state.start = true;
                    return 0;
                //ENTER
                case 0x1C:
                    if (selectorPause == 0){
                        game->state.mode = SINGLEPLAYER;
                        game->state.start = true;
                        current_menu = game_background;
                        current_menu_img = game_background_img;
                        return 0;
                        
                    }
                    if(selectorPause == 1){
                        game->state.mode = EXIT;
                        game->state.start = true;
                        return 0;
                    }
                    default:
                        return 0;
            }
        case SCOREBOARD:
            return 0;
        case EXIT:
            return 0;
        case WIN:
        switch(code){
                //ARROW DOWN
                case 0x48:
                    selectorWin = selectorWin-1;
                    if(selectorWin < 0) selectorWin = 1;
                    game->state.draw = true;
                    return 0;
                //ARROW UP
                case 0x50:
                    selectorWin = (selectorWin+1)%2; 
                    game->state.draw = true;
                    return 0;
                //ENTER
                case 0x1C:
                    if (selectorWin == 0){
                        game->state.mode = MENU;
                        game->state.start = true;
                        game->state.draw = true;
                        current_menu = menu_start;
                        current_menu_img = menu_start_img;
                        return 0;
                        
                    }
                    if(selectorWin == 1){
                       game->state.mode = EXIT;
                        return 0;
                    }
                    default:
                        return 0;
            }
        game->state.draw = true;
            return 0;
    }
}


int singlePlayer_mode(letter ** sentence,letter ** sentence2, letter **inputSentence, unsigned int idx){
    if(game->state.start){
        if(game->state.sentence) {
            printf("%u", counter_timer);
            game_time = counter_timer;
        }
        game->state.draw = true;
        game->state.drawInput = true;
        singlePlayer_start(sentence, inputSentence);
        game->state.start = false;
    }
    else{
        if(game->state.clean){
            if(clear_xpm_with_cover(100, 686,70,66,game_background_img,game_background)){
            vg_exit();
            printf("%s: Error drawing xpm", __func__);
            return 1;
            }
            game->state.clean = false;
        }
        if(isUpper){
            if(vg_draw_xpm(100, 686, upper_img, upper)){
                vg_exit();
                printf("%s: Error drawing xpm\n", __func__);
                return 1;
            }
        }
        if(game->state.sentence == 1){
            if(draw_sentence(*sentence,get_hres()/2.0-350 ,get_vres()/2.0-250)){
                vg_exit();
                printf("%s: Error drawing rectangle\n", __func__);
            return 1;
            }
        }
        else{
            if(draw_sentence(*sentence2,get_hres()/2.0-350 ,get_vres()/2.0-250)){
                vg_exit();
                printf("%s: Error drawing rectangle\n", __func__);
            return 1;
            }
        }
        
        double_buffering();
        if(game->state.drawInput){
             if(clear_xpm_with_cover(188,396,963-188,592-396,game_background_img,game_background)){
                vg_exit();
                printf("%s: Error drawing xpm", __func__);
                return 1;
                }
            
        }
        if(game->state.sentence == 1){
            if(draw_input_sentence(*inputSentence,*sentence,get_hres()/2.0-350 ,get_vres()/2.0-20,idx)){
                vg_exit();
                printf("%s: Error drawing rectangle\n", __func__);
                return 1;
            }

            }
            else{
                if(draw_input_sentence(*inputSentence,*sentence2,get_hres()/2.0-350 ,get_vres()/2.0-20,idx)){
                vg_exit();
                printf("%s: Error drawing rectangle\n", __func__);
                return 1;
            }
     
        }
        double_buffering();

        if(game->state.drawInput){
            if(clear_xpm_with_cover(1072, 786,30,39,game_background_img,game_background)){
                vg_exit();
                printf("%s: Error drawing xpm", __func__);
                return 1;                    
            }
           
        }
        bool equal = true;
        
        if(game->state.sentence == 1){
            equal = isEqual(idx,number_Letters1, sentence,inputSentence);
        }
        else if(game->state.sentence == 2){
            equal = isEqual(idx,number_Letters2, sentence2,inputSentence);
        }
        
        if(equal){
            game->state.canAdvance = true;
            if(vg_draw_xpm(1072, 786, next_img, next)){
                vg_exit();
                printf("%s: Error drawing xpm\n", __func__);
                return 1;
            }
        }
        else{
            game->state.canAdvance = false;
        }
        
        game->state.drawInput = false;     
       
    }
    return 0;
}

int win_menu_f(){
    if(game->state.start){
        game_time = (counter_timer - game_time) / 60;
        printf("%u", game_time);
        if(vg_draw_xpm(0, 0, win_menu_img, win_menu)){
                vg_exit();
                printf("%s: Error drawing xpm\n", __func__);
                return 1;
            }
        game->state.start = false;
    }
    else{
        if(game->state.draw){
            if(selectorWin == 0){
                if(vg_draw_xpm(0, 0, win_menu_img, win_menu)){
                vg_exit();
                printf("%s: Error drawing xpm\n", __func__);
         
                return 1;
                }
            }
            else if(selectorWin == 1){
                if(vg_draw_xpm(0, 0, win_exit_img, win_exit)){
                vg_exit();
                printf("%s: Error drawing xpm\n", __func__);
                return 1;
                }
            }
        }
    }
    return 0;
}
int singlePlayer_start(letter **sentence, letter **inputSentence){
    if(game->state.draw){

        if(vg_draw_xpm(0,0, game_background_img, game_background)){
            vg_exit();
            return 1;
        }
        
        
        game->state.draw = false;
    }
    double_buffering();
    return 0;
}

int pause(){
    if(game->state.start){
        game->state.draw = true;
        drawPauseMenu();
        game->state.start = false;
    }
    return 0;
}

int drawPauseMenu(){

    switch (selectorPause){
    case 0:
        if(vg_draw_xpm(0,0, menu_pause_resume_img, menu_pause_resume)){
            vg_exit();
            printf("%s: Error drawing xpm\n", __func__);
            return 1;
        }
        game->state.draw = false;
       
        break;
    case 1:
        if(vg_draw_xpm(0,0, menu_pause_exit_img, menu_pause_exit)){
            vg_exit();
            printf("%s: Error drawing xpm\n", __func__);
            return 1;
        }
        game->state.draw = false;
       
        break;
    
    default:
        break;
    }
    
    double_buffering();
    return 0;
}


int draw_sentence(letter *sentence, uint16_t x, uint16_t y) {
  uint16_t x1=x; /*x1 -> posição onde desenhar a letras*/
  bool finish = false;
  for(int i = 0; !finish; i++){

    if(sentence[i].letter == '.'){
      finish = true;
    }

    uint16_t temp=x1;
    if(sentence[i].letter == ' '){
      //procura o proximo espaço da frase delimitando a palavra
        for(int j = i+1; true; j++){
            if(sentence[j].letter == ' ' || sentence[j].letter == '.'){// encontrando um espaço
            //calcula se a proxima palavra cabe no ecra 
            for(int k = i+1; k <= j; k++){ 
                temp += sentence[k].img.width + 1;
            }
            if(temp > x + 680){
                y += sentence[i].img.height - 2;
                x1=x;
            }
            break;
            }
        }
    }
  
    if((x1-x)>700){
      y += sentence[i].img.height + 1;
      x1=x;
    }
    if(sentence[i].letter != ' '){
      vg_draw_xpm(x1,y,sentence[i].img,sentence[i].xpm);
      
      x1+=sentence[i].img.width + 1;
    }
    else{
      if(x1 != x){
        vg_draw_xpm(x1,y,sentence[i].img,sentence[i].xpm);
        x1+=sentence[i].img.width + 1;
      }
    }
  }
  return 0;
}



int draw_input_sentence(letter *input,letter *sentence, uint16_t x, uint16_t y, int size) {

  uint16_t x1=x; /*x1 -> posição onde desenhar a letras*/
  bool finish = false;
    

  for(int i = 0; i < size; i++){ 
   
    if(i == size-1){
      finish = true;
    }
   
    uint16_t temp=x1;
    if(sentence[i].letter == ' '){
      for(int j = i+1; true; j++){
      if(sentence[j].letter == ' ' || sentence[j].letter == '.'){
        for(int k = i+1; k <= j; k++){ 
          temp += sentence[k].img.width + 1;
        }
        if(temp > x + 700){
          y +=sentence[i].img.height - 2;
          x1=x;
        }
        break;
       
      }
    }
    }
  
    if(input[i].letter != ' '){
        vg_draw_xpm(x1,y,input[i].img,input[i].xpm);
        x1+=input[i].img.width + 1;
    }
    else{
      if(x1 != x){
        vg_draw_xpm(x1,y,input[i].img,input[i].xpm);
      
        x1+=input[i].img.width + 1;
      }
    }
  }
  return 0;
}


void generateSentence(letter ** sentence, letter ** sentence2){

    int number = 0;
    time_t t;
    srand(time(&t));
    unsigned x = (rand() % (7 - 0 + 1)) + 0;
    
    while(sentences[x][number].letter !=  '.') 
        number++;
    
    number++;
    number_Letters1 = number;
    *sentence = (letter*)malloc(number*sizeof(letter));

    memcpy(*sentence,sentences[x], number*sizeof(letter));

    unsigned i = x;
    while(i == x){
        i = (rand() % (7 - 0 + 1)) + 0;
    }


    number = 0;
    while(sentences[i][number].letter !=  '.') 
        number++;

    number++;
    number_Letters2 = number;
    *sentence2 = (letter*)malloc(number*sizeof(letter));

    memcpy(*sentence2,sentences[i], number*sizeof(letter));

}

void allocateSentence(letter ** sentence){
    int number = 500;
    *sentence = (letter*)malloc(number*sizeof(letter));
}

bool isEqual(unsigned int idx,unsigned int number_Letters, letter ** sentence,letter **inputSentence){
    if(number_Letters == idx){
        
        for(unsigned int i = 0; i < idx; i++){
            if((*sentence)[i].letter != (*inputSentence)[i].letter){ 
                return false;
                }

        }
        return true;
    }
    return false;
}
