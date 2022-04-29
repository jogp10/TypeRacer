#include <lcom/lcf.h>
#include "vc.h"

#include <math.h>
#include <stdint.h>

#include "vc_macros.h"

static void *video_mem; /* frame-buffer VM address */

static vbe_mode_info_t info; /* VBE information on input mode */

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
