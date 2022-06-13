#include "vg.h"
#include "vg_macros.h"
#include "xpms/xpm.h"
#include <lcom/lcf.h>
#include <math.h>

static void *video_mem;         /* VBE information on input mode */
char *double_buf;

vbe_mode_info_t info;           /* VBE information on input mode */
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel;
static unsigned size;

xpm_image_t mouse_img;
xpm_image_t menu_start_img;
xpm_image_t menu_single_img;
xpm_image_t menu_multi_img;
xpm_image_t menu_rules_img;
xpm_image_t menu_leave_img;
xpm_image_t menu_pause_img;
xpm_image_t menu_pause_resume_img;
xpm_image_t menu_pause_exit_img;
xpm_image_t game_background_img;
xpm_image_t red_car_img;
xpm_image_t next_img;
xpm_image_t upper_img;
xpm_image_t win_menu_img;
xpm_image_t win_exit_img;
xpm_image_t regras_img;
uint8_t *mouse_cursor;
uint8_t *menu_start;
uint8_t *menu_single;
uint8_t *menu_multi;
uint8_t *menu_rules;
uint8_t *menu_leave;
uint8_t *menu_pause;
uint8_t *menu_pause_resume;
uint8_t *menu_pause_exit;
uint8_t *game_background;
uint8_t *red_car;
uint8_t *next;
uint8_t *upper;
uint8_t *win_menu;
uint8_t *win_exit;
uint8_t *regras;

int(load_all_xpms)(){
  // get the pixmap from the XPM
  mouse_cursor = xpm_load(mouse_cursor_xpm, XPM_8_8_8_8, &mouse_img);
  if(mouse_cursor == NULL){
    printf("mouse cursor no load");
    return 1;
  }

  menu_start = xpm_load(menu_xpm, XPM_8_8_8_8, &menu_start_img);
  if(menu_start == NULL){
    printf("start menu no load");
    return 1;
  }

  menu_single = xpm_load(menu_single_xpm, XPM_8_8_8_8, &menu_single_img);
  if(menu_single == NULL){
    printf("single menu no load");
    return 1;
  }

  menu_rules = xpm_load(menu_rules_xpm, XPM_8_8_8_8, &menu_rules_img);
  if(menu_rules == NULL){
    printf("rules menu no load");
    return 1;
  }
   
  menu_leave = xpm_load(menu_leave_xpm, XPM_8_8_8_8, &menu_leave_img);
  if(menu_leave == NULL){
    printf("leave menu no load");
    return 1;
  }

  menu_multi = xpm_load(menu_multi_xpm, XPM_8_8_8_8, &menu_multi_img);
  if(menu_multi == NULL){
    printf("multi menu no load");
    return 1;
  }

  menu_pause = xpm_load(pause_menu_xpm, XPM_8_8_8_8, &menu_pause_img);
  if(menu_pause == NULL){
    printf("pause menu no load");
    return 1;
  }

  menu_pause_resume = xpm_load(pause_menu_resume_xpm, XPM_8_8_8_8, &menu_pause_resume_img);
  if(menu_pause_resume == NULL){
    printf("pause resume menu no load");
    return 1;
  }

  win_menu = xpm_load(win_menu_xpm, XPM_8_8_8_8, &win_menu_img);
  if(win_menu == NULL){
    printf("win menu no load");
    return 1;
  }

  win_exit = xpm_load(win_exit_xpm, XPM_8_8_8_8, &win_exit_img);
  if(win_exit == NULL){
    printf("win_exit no load");
    return 1;
  }

  menu_pause_exit = xpm_load(pause_menu_exit_xpm, XPM_8_8_8_8, &menu_pause_exit_img);
  if(menu_pause_exit == NULL){
    printf("pause exit menu no load");
    return 1;
  }

  game_background = xpm_load(background_xpm, XPM_8_8_8_8, &game_background_img);
  if(game_background == NULL){
    printf("game background no load");
    return 1;
  }

  red_car = xpm_load(red_car_xpm, XPM_8_8_8_8, &red_car_img);
  if(red_car == NULL){
    printf("red car no load");
    return 1;
  }

  next = xpm_load(next_xpm, XPM_8_8_8_8, &next_img);
  if(next == NULL){
    printf("next no load");
    return 1;
  }

  upper = xpm_load(upper_xpm, XPM_8_8_8_8, &upper_img);
  if(upper == NULL){
    printf("upper no load");
    return 1;
  }

  regras = xpm_load(regras_xpm, XPM_8_8_8_8, &regras_img);
  if(regras == NULL){
    printf("regras no load");
    return 1;
  }

  return 0;

}

int (vg_change_mode)(uint16_t mode)
{   
    if(map_memory(mode) != OK){
        printf("%s: Error maping memory", __func__);
        return 1;
    }

    reg86_t reg86;
    memset(&reg86, 0, sizeof(struct reg86));
    reg86.intno = VBE_INT;
    reg86.ah = VBE_SUPPORTED;
    reg86.al = VBE_VG;
    reg86.bx = mode | VBE_VIDEO;
    

    if(sys_int86(&reg86) != OK){
        printf("%s: sys_int86() failed \n", __func__);
        return 1;
    }
    return 0;
}


int (map_memory)(uint16_t mode){

    if(vbe_get_mode_info(mode, &info) != OK){
        printf("%s: vbe_get_mode_info() failed \n", __func__);
        return 1;
    }
    
    h_res = info.XResolution;
    v_res = info.YResolution;
    bits_per_pixel = info.BitsPerPixel;
    bytes_per_pixel = ceil(bits_per_pixel/8.0);
    size = (h_res * v_res * bytes_per_pixel);

    struct minix_mem_range mr;
    unsigned int vram_base = info.PhysBasePtr; /* VRAM's physical addresss */
    unsigned int vram_size = h_res * v_res * bytes_per_pixel; /* VRAM's size, but you can use the frame-buffer size, instead */
    int r;

    /* Allow memory mapping */
    mr.mr_base = (phys_bytes)vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)) != OK){
        panic("Sys_privctl (ADD_MEM) failed: %d\n", r);
        return 1;
    }

    /* Map memory */
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if (video_mem == MAP_FAILED){
        panic("Couldn't map video memory");
        return 1;
    }

    double_buf = (char*) malloc(vram_size);

    return 0;

}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (unsigned i = 0; i < height; i++) {
        if (vg_draw_hline(x, y + i, width, color)) {
            printf("Error drawing line.\n");
            return 1;
        }
    }
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t width, uint32_t color) {
    for (int i = 0; i < width; i++) {
        if (vg_draw_pixel(x + i, y, color)) {
            printf("Error drawing pixel.\n");
            return 1;
        }
    }
    return 0;
}


int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    if (x < 0 || x >= h_res || y >= v_res || y < 0 ) return 0;
    uint8_t *pixel = (uint8_t *) double_buf + (( (y * h_res) + x ) * bytes_per_pixel);

    memcpy(pixel, &color, bytes_per_pixel);

    return 0;
}


uint8_t (R)(uint32_t color) {return color >> info.RedFieldPosition % BIT(info.RedMaskSize);}
uint8_t (G)(uint32_t color) {return color >> info.GreenFieldPosition % BIT(info.GreenMaskSize);}
uint8_t (B)(uint32_t color) {return color >> info.BlueFieldPosition % BIT(info.BlueMaskSize);}


int (vg_draw_xpm)(uint16_t x, uint16_t y, xpm_image_t img, uint8_t *map) {

  // copy it to graphics memory

  if (map == NULL){
    vg_exit();
    printf("Error getting pixmap.\n");
    return 1;
  }


  for (unsigned int i=0; i<img.width; i++) {
    for (unsigned int j=0; j<img.height; j++) {
      uint32_t color;
      memcpy(&color, map + (j * img.width + i) * bytes_per_pixel, bytes_per_pixel);
      if (color != xpm_transparency_color(img.type)) vg_draw_pixel(x + i, y + j, color);
    }
  }

  return 0;
}


int (vg_clean_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

  xpm_image_t img;
  uint8_t *map;
  // get the pixmap from the XPM
  map = xpm_load(xpm, XPM_8_8_8_8, &img);
  // copy it to graphics memory

  if (map == NULL){
    printf("Error getting pixmap.\n");
    return 1;
  }

  unsigned int index = 0;

  for (unsigned int i=0; i<img.height; i++) {
    for (unsigned int j=0; j<img.width; j++) {
      uint32_t color = 0;
      index++;
      vg_draw_pixel(x + j, y + i, color);
    }
  }

  return 0;
}

int get_hres(){
  return h_res;
}

int get_vres(){
  return v_res;
}

void (double_buffering)() {
  memcpy(video_mem, double_buf, size);
}

char* (get_double_buffer)() {
  return double_buf;
}

int clear_xpm_with_cover(uint16_t x, uint16_t y, uint16_t maxX, uint16_t maxY, xpm_image_t background_img, uint8_t *map) {

  if (map == NULL){
    printf("Error getting pixmap.\n");
    return 1;
  }

  unsigned int index = 0;

  for (unsigned int i=x; i<maxX+x; i++) {
    
    for (unsigned int j=y; j<maxY+y; j++) {
      uint32_t color = 0;
      index++;
       memcpy(&color, map + (j * background_img.width + i) * bytes_per_pixel, bytes_per_pixel);
      if (color != xpm_transparency_color(background_img.type)) vg_draw_pixel(i,j, color);
     
    }
  }
  return 0;
}

