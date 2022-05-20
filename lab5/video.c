#include <lcom/lcf.h>
#include <stdint.h>
#include "minix/sysutil.h"
#include "video.h"

static void *video_mem;		        /* Process (virtual) address to which VRAM is mapped */
static unsigned int h_res;	        /* Horizontal resolution in pixels */
static unsigned int v_res;	        /* Vertical resolution in pixels */
static unsigned int bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned int vram_base;      /* VRAM's physical addresss */
static unsigned int vram_size;      /* VRAM's size, but you can use the frame-buffer size, instead*/


int (video_set_graphics_mode)(uint16_t mode){

    reg86_t reg86;

    /* zero the structure */
    memset(reg86, 0, sizeof(*reg86)); 

    reg86.intno = 0x10;
    reg86.ah = 0x4F;
    reg86.al = mode;

    if(sys_int86(reg86) != OK){
        printf("%s: sys_int86() failed \n", __func__);
        return 1;
    }

    return 0;
}

int(video_map_addr)(endpoint_t who, void *phaddr, size_t len){

    //[...]

    //[...]

    struct minix_mem_range mr;
    int r;				    

    /* Use VBE function 0x01 to initialize vram_base and vram_size */
    vbe_mode_info_t* vmi;
    vbe_get_mode_info (RETURN_MODE, vmi);
    h_res = vmi->XResolution;
    v_res = vmi->YResolution;
    bits_per_pixel = vmi->BitsPerPixel;
    vram_base = ;
    vram_size = ;

    /* Allow memory mapping */
    mr.mr_base = (phys_bytes) vram_base;	
    mr.mr_limit = mr.mr_base + vram_size;  

    if( OK != (r = sys_privctl(who, SYS_PRIV_ADD_MEM, &mr))){
        panic("%s: sys_privctl (ADD_MEM) failed: %d\n", __func__, r);
        return 1;    
    }

    /* Map memory */
    video_mem = vm_map_phys(who, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED){
        panic("%s: couldn't map video memory", __func__);
        return 1;
    }

}
