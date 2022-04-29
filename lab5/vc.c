#include <lcom/lcf.h>
#include "vc.h"

#include <stdint.h>

#include "vc_macros.h"

static void *video_mem; /* frame-buffer VM address */

static char *vram;		/* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int (vc_change_mode)(uint16_t mode) {
  reg86_t r;

  
  /* Specify the appropriate register values */
  memset(&r, 0, sizeof(r)); /* zero the stucture */

  r.intno = 0x10;     /* BIOS video services */
  r.ah = 0x4F;        /* VBE call */
  r.al = 0x02;         /* Set VBE function */
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

int (map_memory)() {
  struct minix_mem_range mr;
  unsigned int vram_base;
  unsigned int vram_size;

  int r;

  /* Use VBE function 0x01 to initialize vram_base and vram_size */
  vbe_get_mode_info(, vram);

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size; 

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)) {
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
