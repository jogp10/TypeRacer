#include <lcom/lcf.h>
#include "vc.h"

#include <stdint.h>

#include "vc_macros.h"

//static void *video_mem; /* frame-buffer VM address */

int (vc_change_mode)(uint16_t mode) {
  reg86_t r;

  
  /* Specify the appropriate register values */
  memset(&r, 0, sizeof(r)); /* zero the stucture */

  r.intno = 0x10;     /* BIOS video services */
  r.ah = 0x4F;        /* initialize VBE mode */
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
  
  return 0;
}
