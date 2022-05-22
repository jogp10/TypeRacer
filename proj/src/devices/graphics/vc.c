#include "vc.h"
#include <math.h>
#include <stdint.h>
#include "vc_macros.h"

static void *video_mem;         /* frame-buffer VM address */

static vbe_mode_info_t info;    /* VBE information on input mode */

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int (vc_change_mode)(uint16_t mode) {
  reg86_t r;

  if (vc_get_mode_info(mode, &info)) {
    printf("Error getting minix mode info.\n");
    return 1;
  }

  if (map_memory()) {
    printf("Error mapping memmory.\n");
    return 1;
  }

  /* Specify the appropriate register values */
  memset(&r, 0, sizeof(r)); /* zero the stucture */

  r.intno = 0x10;     /* BIOS video services */
  r.ah = 0x4F;        /* VBE call */
  r.al = 0x02;         /* Set VBE mode function */
  r.bx = BIT(14) | mode; /* Indexed Color Model, 8 bits per pixel */

  /* Make the BIOS call */
  if (sys_int86(&r) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }

  /* Function not supported or succesful */
  if (r.ah != 0x00 || r.al != 0x4F) {
      printf("Error in function call.\n");
      return 1;
  }

  return 0;
}

int (vc_get_mode_info)(uint16_t mode, vbe_mode_info_t *info) {
  reg86_t r;

  /* Specify the appropriate register values */
  memset(&r, 0, sizeof(r)); /* zero the stucture */

  mmap_t mem_map;
  if (lm_alloc(sizeof(vbe_mode_info_t), &mem_map) == NULL) {
    printf("Failed to allocate mem, vc_get_mode_info.\n");
    return 1;
  }

  r.intno = 0x10;     /* BIOS video services */
  r.ax = 0x4F01;        /* VBE call */
  r.cx = mode; /* Indexed Color Model, 8 bits per pixel */
  r.es = PB2BASE(mem_map.phys); /* Pointer to ModeInfoBlock struct */
  r.di = PB2OFF(mem_map.phys);


  /* Make the BIOS call */
  if (sys_int86(&r) != OK) {
    printf("vc_get_mode_info: sys_int86() failed \n");
    return 1;
  }

    /* Function not supported or succesful */
  if (r.ah != 0x00 || r.al != 0x4F) {
      printf("Error in function call.\n");
      lm_free(&mem_map);
      return 1;
  }

  *info = *(vbe_mode_info_t *)mem_map.virt;
  lm_free(&mem_map);
  return 0;
}

int (map_memory)() {
  h_res = info.XResolution;
  v_res = info.YResolution;
  bits_per_pixel = info.BitsPerPixel;

  struct minix_mem_range mr;
  unsigned int vram_base = info.PhysBasePtr;
  unsigned int vram_size = h_res * v_res * ceil(bits_per_pixel / 8.0);

  int r;


  /* Allow memory mapping */
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privct1 (ADD_MEM) failed: %d\n", r);
    return 1;
  }

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return 1;
  }
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (uint16_t j=0; j<height; j++) {
    if (j + y >= v_res) {
      printf("End of screen");
      break; // end of screen
    }
    if (vg_draw_hline(x, y + j, width, color)) {
      printf("Failed to draw line (%d, %d)", x, j);
      return 1;
    }
  }
  return 0;
}


int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if ( !(x < h_res && y < v_res)) {
    printf("End of screen");
    return 0;
  }

  /** pixel address */
  uint8_t *pixel_add = (uint8_t *) video_mem + ((int) ceil(bits_per_pixel / 8.0) * (h_res*y + x) );

  if ( (info.MemoryModel != DIRECT_COL_MODEL_1) && (info.MemoryModel != DIRECT_COL_MODEL_2)) {
    color &= 0xFFFF;
  } else color &= 0xFFFFFF;
  
  uint8_t tmp;

  for (uint i = 0; i < ceil (bits_per_pixel / 8); i++) {
    tmp = color & 0xFF;
    *(pixel_add + i) = tmp;
    color = color >> 8;
  }

  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (uint16_t i=0; i<len; i++) {
    if (i + x > h_res) {
      printf("End of screen");
      break; // end of screen
    }
    if (vg_draw_pixel(x + i,  y, color)) {
      printf("Failed to draw pixel (%d, %d)", i, y);
      return 1;
    }
  }
  return 0;
}

int (vg_draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

  xpm_image_t img;
  uint8_t *map;
  // get the pixmap from the XPM
  map = xpm_load(xpm, XPM_INDEXED, &img);
  // copy it to graphics memory

  if (map == NULL){
    printf("Error getting pixmap.\n");
    return 1;
  }

  uint index = 0;

  for (unsigned int i=0; i<img.height; i++) {
    for (unsigned int j=0; j<img.width; j++) {
      uint32_t color = map[index];
      index++;
      vg_draw_pixel(x + j, y + i, color);
    }
  }

  return 0;
}

int (vg_clean_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

  xpm_image_t img;
  uint8_t *map;
  // get the pixmap from the XPM
  map = xpm_load(xpm, XPM_INDEXED, &img);
  // copy it to graphics memory

  if (map == NULL){
    printf("Error getting pixmap.\n");
    return 1;
  }

  uint index = 0;

  for (unsigned int i=0; i<img.height; i++) {
    for (unsigned int j=0; j<img.width; j++) {
      uint32_t color = 0;
      index++;
      vg_draw_pixel(x + j, y + i, color);
    }
  }

  return 0;
}

int (vg_move_xpm)(xpm_map_t xpm, uint16_t *xi, uint16_t *yi, uint16_t xf, uint16_t yf, uint16_t speed) {
  if (vg_clean_xpm(xpm, *xi, *yi)) {
    printf("Failed to erase xpm\n");
    return 1;
  }

  if (xf!=*xi) {
    if(abs(xf-*xi)<speed) speed = abs(xf-*xi);
    if(xf-*xi>0) *xi = *xi + speed;
    else *xi = *xi - speed;
  }

  if (yf!=*yi) {
    if(abs(yf-*yi)<speed) speed = abs(yf-*yi);
    if(yf-*yi>0) *yi = *yi + speed;
    else *yi = *yi - speed;
  }


  if (vg_draw_xpm(xpm, *xi, *yi)) {
    printf("Failed to draw xpm\n");
    return 1;
  }
  return 0;
}
