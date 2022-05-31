#include "vg.h"
#include "vg_macros.h"

#include <math.h>

static void *video_mem;         /* VBE information on input mode */

vbe_mode_info_t info;           /* VBE information on input mode */
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int (vg_change_mode)(uint16_t mode) {
    reg86_t r86;
    
    // Information on mode
    if (vbe_get_info_mode(mode, &info)) {
        printf("Error getting information on mode.\n");
        return 1;
    }

    // Map memory
    if (map_memory()) {
        printf("Error mapping memmory.\n");
        return 1;
    }

    // Change to video graphics mode
    memset(&r86, 0, sizeof(r86));

    r86.intno = VBE_INT;
    r86.ah = VBE_CALL;
    r86.al = VBE_VG;
    r86.bx = VBE_VIDEO | mode;

    if (sys_int86(&r86)) {
        printf("call to system error.\n");
        return 1;
    }

    if (r86.al != VBE_SUPPORTED) {
        printf("VBE Function not supported.\n");
        return 1;
    }
    if (r86.ah != VBE_SUCCESS) {
        printf("VBE Function failed.\n");
        return 1;
    }
    return 0;
}

int (map_memory)() {
    struct minix_mem_range mr;
    unsigned int vram_base = info.PhysBasePtr;
    unsigned int vram_size = v_res * h_res * (bits_per_pixel / 8.0);
    int r;

    /* Allow memory mapping */
    mr.mr_base = (phys_bytes) vram_base;	
    mr.mr_limit = mr.mr_base + vram_size;  

    if( (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
        return 1;
    }

    /* Map memory */
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED) {
        panic("couldn't map video memory");
        return 1;
    }
    return 0;
}

int (vbe_get_info_mode)(uint16_t mode, vbe_mode_info_t *info) {
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
  if (sys_int86(&r)) {
    printf("vc_get_mode_info: sys_int86() failed \n");
    return 1;
  }

    /* Function not supported or succesful */
  if (r.ah != VBE_SUCCESS || r.al != VBE_SUPPORTED) {
      printf("Error in function call.\n");
      lm_free(&mem_map);
      return 1;
  }

  *info = *(vbe_mode_info_t *)mem_map.virt;

  h_res = info->XResolution;
  v_res = info->YResolution;
  bits_per_pixel = info->BitsPerPixel;

  lm_free(&mem_map);
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

    uint8_t *pixel = (uint8_t *) video_mem + (( (y * h_res) + x ) * (int) ceil(bits_per_pixel / 8.0));

    for (int i = 0; i < (int) ceil(bits_per_pixel / 8.0); i++) {
        *pixel = color;
        color >>= 8;
        pixel++;
    }

    return 0;
}

uint8_t (R)(uint32_t color) {return color >> info.RedFieldPosition % BIT(info.RedMaskSize);}
uint8_t (G)(uint32_t color) {return color >> info.GreenFieldPosition % BIT(info.GreenMaskSize);}
uint8_t (B)(uint32_t color) {return color >> info.BlueFieldPosition % BIT(info.BlueMaskSize);}


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
