#include <lcom/lcf.h>

int (vg_change_mode)(uint16_t mode);

int (map_memory)();

int (vbe_get_info_mode)(uint16_t mode, vbe_mode_info_t *info);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t width, uint32_t color);

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

uint8_t (R)(uint32_t color);
uint8_t (G)(uint32_t color);
uint8_t (B)(uint32_t color);
