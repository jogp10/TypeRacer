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
int selectorMenu = 0, selectorPause = 0;
static StartMenuEntry startMenuEntry;
static StartMenuEntry prevStartMenuEntry;

extern unsigned int counter_kbd, counter_timer;
extern uint8_t code;

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

Point singlePlayerOpt[] = {{408,452}, {530,430}, {524,334}, {406,360}};
Point multiPlayerOpt[] = {{538,430}, {680,406}, {670,300}, {532,330}};
Point rulesOpt[] = {{700,402}, {866,376}, {852,260}, {690,296}};
Point leaveOpt[] = {{880,372}, {1080,338}, {1056,212}, {866,256}};

xpm_image_t current_menu_img;
uint8_t *current_menu;

int game_init(Game *self) {
    unsigned int idx=0;
    letter *sentence= NULL;
    letter *inputSentence = NULL;
    inputSentence = (letter*)malloc(136*sizeof(letter));

    current_menu_img = menu_start_img;
    current_menu = menu_start;
    startMenuEntry = DEFAULT;
    prevStartMenuEntry = DEFAULT;

    load(); 
    loadSentences();
    generateSentence(&sentence);
  
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

    menu();
    do {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            //printf("driver_receive failed with: %d\n", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & BIT(timer_bit_no)) { /* timer interrupt */
                        /* process it */
                        timer_int_handler();
                        //printf("timer\n");               
                        switch (game->state.mode){
                            case MENU:
                                changeMenuState();
                                break;
                            case SINGLEPLAYER:
                                if(singlePlayer_mode(&sentence, &inputSentence,idx)) return 1;
                                break;
                            case MULTIPLAYER:
                                //Under construction
                                break;
                            case RULES_CREDITS:
                                //Under construction
                                break;
                            case PAUSE:
                                if(pause()) return 1;
                            case SCOREBOARD:
                                //Under construction
                                break;
                            case EXIT:
                                break;
                        }
                        drawMouse();
                    }
                    if (msg.m_notify.interrupts & BIT(mouse_bit_no)) {  //mouse interrupt
                        mouse_ih();
                        //printf("mouse\n");
                        if (mouse_packet_complete(packets, &size_mouse)) {
                            build_packet_struct(packets, &pp);
                            size_mouse = 1;
                            //mouse_print_packet(&pp);
                            mouse_handler(&pp);
                        }
                    }
                    if (msg.m_notify.interrupts & BIT(kb_bit_no)){ /* keyboard interrupt */
                        /* process it */
                        kbd_ih();
                        //printf("keyboard\n");                        
                        if( kbd_code_complete(scan_code, &size_kbd) ) {
                            //kbd_print_scancode(!(code & MAKE_CODE), size_kbd, scan_code);
                            kbd_handler(sentence,&inputSentence,&idx);
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

    return 0;
}

void changeMenuState(){
    if(counter_timer % 30 == 0){
        //printf("Prev Entry: %d\n", prevStartMenuEntry);
        //printf("Current Entry: %d\n", startMenuEntry);
    }
    if(startMenuEntry == prevStartMenuEntry){
        return;
    }
    switch (startMenuEntry){
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
    double_buffering();

}

void mouse_handler(struct packet * p){
    game->mouse.lmb = p->lb;
    /*if(game->state.mode == SINGLEPLAYER){
        if(clear_xpm_with_cover(game->mouse.mouse_x,game->mouse.mouse_y, 15,26,game_background_img, game_background)){
            vg_exit();
            printf("%s: Error drawing rectangle\n", __func__);
            return;
        }
    }else if(game->state.mode == MENU){
        uint8_t* xpm;
        xpm_image_t img;
        xpm = menu_start;
        img = menu_start_img;
        if(selectorMenu == 0){
            xpm = menu_single;
            img = menu_single_img;
        }
        else if(selectorMenu == 1){
            xpm = menu_multi;
            img = menu_multi_img;
        }
        else if(selectorMenu == 2){
            xpm = menu_rules;
            img = menu_rules_img;
        }    
        else if(selectorMenu == 3){
            xpm = menu_leave;
            img = menu_leave_img;
        }


        if(clear_xpm_with_cover(game->mouse.mouse_x,game->mouse.mouse_y, 15,26,img, xpm)){
            vg_exit();
            printf("%s: Error drawing rectangle\n", __func__);
            return;
        }
    }*/

    clear_xpm_with_cover(game->mouse.mouse_x,game->mouse.mouse_y, 15,26,current_menu_img, current_menu);

    if(game->mouse.mouse_x + p->delta_x >= 0 && game->mouse.mouse_x + p->delta_x + 15 <= (int) get_hres()){
        game->mouse.mouse_x += p->delta_x;
    }
    if(game->mouse.mouse_y - p->delta_y >= 0 && game->mouse.mouse_y - p->delta_y + 26 <= (int) get_vres()){
        game->mouse.mouse_y -= p->delta_y;
    }

    
    Point mouse_pt = {game->mouse.mouse_x,game->mouse.mouse_y};
    if(isInside(singlePlayerOpt, 4, mouse_pt)){
        prevStartMenuEntry = startMenuEntry;
        startMenuEntry = SINGLE;
    }
    else if(isInside(multiPlayerOpt, 4, mouse_pt)){
        prevStartMenuEntry = startMenuEntry;
        startMenuEntry = MULTI;
    }
    else if(isInside(rulesOpt, 4, mouse_pt)){
        prevStartMenuEntry = startMenuEntry;
        startMenuEntry = RULES;
    }
    else if(isInside(leaveOpt, 4, mouse_pt)){
        prevStartMenuEntry = startMenuEntry;
        startMenuEntry = EXIT_GAME;
    }
    printf("Selected: %d\n", startMenuEntry);
    
}

int drawMouse(){  
    if(vg_draw_xpm(game->mouse.mouse_x, game->mouse.mouse_y, mouse_img, mouse_cursor)){
        vg_exit();
        //printf("%s: Error drawing xpm\n", __func__);
        return 1;
    }
    double_buffering();
    return 0;
}

int menu() {
    game->mouse.lmb = false;

    //printf("a desenhar menu\n");

    if(vg_draw_xpm(0,0, menu_start_img, menu_start)){
        vg_exit();
        //printf("%s: Error drawing xpm", __func__);
        return 1;
    }
    /*switch (selectorMenu){
    case 0:
        if(vg_draw_xpm(0,0, menu_single_img, menu_single)){
            vg_exit();
            printf("%s: Error drawing xpm\n", __func__);
            return 1;
        }
       
        break;
    case 1:
        if(vg_draw_xpm(0,0, menu_multi_img, menu_multi)){
            vg_exit();
            printf("%s: Error drawing xpm\n", __func__);
            return 1;
        }
        break;
    case 2:
        if(vg_draw_xpm(0,0, menu_rules_img, menu_rules)){
            vg_exit();
            printf("%s: Error drawing xpm\n", __func__);
            return 1;
        }
        break;
    case 3:
        if(vg_draw_xpm(0,0, menu_leave_img, menu_leave)){
            vg_exit();
            printf("%s: Error drawing xpm\n", __func__);
            return 1;
        }
        break;
    default:
        break;
    }*/
    

    // draw rectangle highlights
    //if (game->mouse.mouse_x >= 0 && game->mouse.mouse_x <= (int) get_hres()) {}


    /*if(vg_draw_xpm(game->mouse.mouse_x, game->mouse.mouse_y, mouse_img, mouse_cursor)){
        vg_exit();
        printf("%s: Error drawing xpm\n", __func__);
        return 1;
    }*/

    double_buffering();
    
    return 0;
}


/*int drawPauseMenu() {
    if(vg_draw_rectangle(0, 0, 1152, 864, 0x1F)){
        vg_exit();
        printf("%s: Error drawing rectangle\n", __func__);
        return 1;
    }
    
    if(vg_draw_xpm(pause_menu_xpm, (1152-686)/2, (864-570)/2)){
        vg_exit();
        printf("%s: Error drawing xpm\n", __func__);
        return 1;
    }

    // draw rectangle highlights
    //if (game->mouse.mouse_x >= 0 && game->mouse.mouse_x <= (int) get_hres()) {}


   

    //buffering();
    
    return 0;
}*/

void nextStartSelected(){  
    
    prevStartMenuEntry = startMenuEntry;
    switch (startMenuEntry)
    {
    case DEFAULT:
        startMenuEntry = SINGLE;
        return;
    case SINGLE:
        startMenuEntry = MULTI;
        return;
    case MULTI:
        startMenuEntry = RULES;
        return;
    case RULES:
        startMenuEntry = EXIT_GAME;
        return;
    case EXIT_GAME:
        startMenuEntry = SINGLE;
        return;
    }
}

void prevStartSelected(){

    prevStartMenuEntry = startMenuEntry;
    switch (startMenuEntry)
    {
    case DEFAULT:
        startMenuEntry = EXIT_GAME;
        return;
    case SINGLE:
        startMenuEntry = EXIT_GAME;
        return;
    case MULTI:
        startMenuEntry = SINGLE;
        return;
    case RULES:
        startMenuEntry = MULTI;
        return;
    case EXIT_GAME:
        startMenuEntry = RULES;
        return;
    }

}

int kbd_handler(letter * sentence,letter ** inputSentence, unsigned int * idx){
  
    switch (game->state.mode){
        case MENU:
            switch(code){
                //ARROW LEFT
                case 0x4B:
                    prevStartSelected();
                    game->state.start = true;
                    //printf("left\n");
                    return 0;
                //ARROW RIGHT
                case 0x4D:
                    nextStartSelected();
                    game->state.start = true;
                    //printf("right\n");
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
                            return 0;
                        case EXIT_GAME:
                            game->state.mode = EXIT;
                            return 0;
                    }
            }    
        case SINGLEPLAYER:
            switch(code){
                //W (???)
                case ESC_BREAK:
                    game->state.mode = PAUSE;
                    game->state.start = true;
                    current_menu = menu_pause_resume;
                    current_menu_img = menu_pause_resume_img;
                    //printf("W\n");
                    return 0;
                //ENTER
                case 0x1C:
                    game->state.mode = MENU;
                    game->state.start = true;
                    current_menu = menu_start;
                    current_menu_img = menu_start_img;
                    //printf("ENTER\n");
                    return 0;
                default:
                    for(unsigned int k =0; k < 57; k++){ 
                        if(letters[k].makeCode == code){ 
                            //printf("%c\n",letters[k].letter);
                            (*inputSentence)[*idx] = letters[k];
                            (*idx)++;
                            //printf("%u\n",*idx);
                            return 0;
                        }
                    }
                    return 0;
            }    
            //validateLetter(*sentence,SCANCODE,input,*index);
        case MULTIPLAYER:
            return 0;
        case RULES_CREDITS:
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
                    //printf("down in pause\n");
                    return 0;
                //ARROW UP
                case 0x50:
                    selectorPause = (selectorPause+1)%2; 
                    game->state.start = true;
                    //printf("UP in pause\n");
                    return 0;
                //ENTER
                case 0x1C:
                    if (selectorPause == 0){
                        game->state.mode = SINGLEPLAYER;
                        game->state.start = true;
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
    }
}


int singlePlayer_mode(letter ** sentence, letter **inputSentence, unsigned int idx){
    if(game->state.start){
        game->state.draw = true;
        
        singlePlayer_start(sentence, inputSentence);
        game->state.start = false;
        /*if(clear_xpm_with_cover(lastX,lastY,lastX+15,lastY+26,menu_single_img,menu_single)){
                vg_exit();
                printf("%s: Error drawing rectangle", __func__);
                return 1;
            }*/
        
    }
    else{
        
        /*printf("anterior:%d,",lastX);
        printf("%d\n",lastY);
        printf("atual:%d,",game->mouse.mouse_x);
        printf("%d\n",game->mouse.mouse_y);
        if(clear_xpm_with_cover(lastX,lastY, 16,25,game_background_img, game_background)){
                vg_exit();
                printf("%s: Error drawing rectangle", __func__);
                return 1;
            }
       if(vg_draw_xpm(game->mouse.mouse_x, game->mouse.mouse_y, mouse_img, mouse_cursor)){
                vg_exit();
                printf("%s: Error drawing xpm", __func__);
                return 1;
            }*/
        if(draw_sentence(*sentence,get_hres()/2.0-350 ,get_vres()/2.0-250,-1)){
            vg_exit();
            //printf("%s: Error drawing rectangle\n", __func__);
            return 1;
        }
    
        if(draw_input(*inputSentence,get_hres()/2.0-350 ,get_vres()/2.0-50,-1,idx)){
            vg_exit();
            //printf("%s: Error drawing rectangle\n", __func__);
            return 1;
        }

    }
    return 0;
}

int singlePlayer_start(letter **sentence, letter **inputSentence){
    if(game->state.draw){

        if(vg_draw_xpm(0,0, game_background_img, game_background)){
            vg_exit();
           //printf("%s: Error drawing xpm\n", __func__);
            return 1;
        }
        
        
        //printf("desenhando jogo\n"); 
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
        //printf("desenhando pausa\n"); 
    }
    else{
      
        

    }
    return 0;
}

int drawPauseMenu(){
    //printf("Selector: %d\n", selectorPause);

    switch (selectorPause)
    {
    case 0:
        if(vg_draw_xpm(0,0, menu_pause_resume_img, menu_pause_resume)){
            vg_exit();
            //printf("%s: Error drawing xpm\n", __func__);
            return 1;
        }
        //printf("desenhado pausa resume\n"); 
        game->state.draw = false;
       
        break;
    case 1:
        if(vg_draw_xpm(0,0, menu_pause_exit_img, menu_pause_exit)){
            vg_exit();
            //printf("%s: Error drawing xpm\n", __func__);
            return 1;
        }
        //printf("desenhado pausa exit\n"); 
        game->state.draw = false;
       
        break;
    
    default:
        break;
    }
    
    //double_buffering();
    return 0;
}


int draw_sentence(letter *sentence, uint16_t x, uint16_t y, int correct) {
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
          //if(sentence[j].letter == '.') break;
        }
    }
    }
  
    if((x1-x)>700){
      y += sentence[i].img.height + 1;
      x1=x;
    }
    if(sentence[i].letter != ' '){

      //draw_letter(sentence[i].xpm,x1,y,sentence[i].img,buff,0xFFFFFF);
      vg_draw_xpm(x1,y,sentence[i].img,sentence[i].xpm);
      
      x1+=sentence[i].img.width + 1;
    }
    else{
      if(x1 != x){
        //draw_letter(sentence[i].xpm,x1,y,sentence[i].img,buff,0xFFFFFF);   
        vg_draw_xpm(x1,y,sentence[i].img,sentence[i].xpm);
        x1+=sentence[i].img.width + 1;
      }
    }
  }
  return 0;
}

int  draw_input(letter *sentence, uint16_t x, uint16_t y, int correct, int size) {
  uint16_t x1=x; /*x1 -> posição onde desenhar a letras*/
  bool finish = false;
  if(size == 0) return 0;
  for(int i = 0; !finish; i++){
    if(i == size){
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
          //if(sentence[j].letter == '.') break;
        }
    }
    }
  
    if((x1-x)>700){
      y += sentence[i].img.height + 1;
      x1=x;
    }
    if(sentence[i].letter != ' '){

      //draw_letter(sentence[i].xpm,x1,y,sentence[i].img,buff,0xFFFFFF);
      vg_draw_xpm(x1,y,sentence[i].img,sentence[i].xpm);
      
      x1+=sentence[i].img.width + 1;
    }
    else{
      if(x1 != x){
        //draw_letter(sentence[i].xpm,x1,y,sentence[i].img,buff,0xFFFFFF);   
        vg_draw_xpm(x1,y,sentence[i].img,sentence[i].xpm);
        x1+=sentence[i].img.width + 1;
      }
    }
  }
  return 0;
}


void generateSentence(letter ** sentence){

    int number = 0;
    time_t t;
    srand(time(&t));
    unsigned x = (rand() % (2 - 0 + 1)) + 0;

    
    while(sentences[x][number].letter !=  '.') number++;
    number++;
    *sentence = (letter*)malloc(number*sizeof(letter));
    memcpy(*sentence,sentences[x], number*sizeof(letter));
}

