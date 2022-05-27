#include "vg.h"
#include "vg_macros.h"

#include <math.h>

static void *video_mem;

vbe_mode_info_t info;
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int (vg_change_mode)(uint16_t mode) {
    // Information on mode
    if (vbe_get_mode_info(mode, &info)) {
        printf("Error getting information on mode.\n");
        return 1;
    }
    h_res = info.XResolution;
    v_res = info.YResolution;
    bits_per_pixel = info.BitsPerPixel;

    // Map memory
    if (map_memory()) {
        printf("Error mapping memmory.\n");
        return 1;
    }

    // Change to video graphics mode
    reg86_t r86;
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
    unsigned int vram_size = v_res * h_res * (ceil(bits_per_pixel / 8));
    uint16_t r;

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

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (int i = 0; i < height; i++) {
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
    if (x < 0 || x > h_res || y > v_res || y < 0 ) return 0;
    uint8_t *pixel = (uint8_t *) video_mem + ( (y * h_res) + x ) * (int) ceil(bits_per_pixel / 8);

    for (int i = 0; i < (int) ceil(bits_per_pixel / 8); i++) {
        *pixel = color;
        color >>= 8;
        pixel++;
    }
    
    return 0;
}
