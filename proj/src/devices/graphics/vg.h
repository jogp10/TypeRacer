#ifndef __VC_H
#define __VC_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief 
 * 
 * @param mode change video graphics using VBE interface to mode
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (vg_change_mode)(uint16_t mode);

/**
 * @brief 
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (map_memory)(uint16_t mode);


/**
 * @brief 
 * 
 * @param x position in x axis to draw
 * @param y position in y axis to draw
 * @param color color to drawn
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

//int (vg_draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);
int (vg_draw_xpm)(uint16_t x, uint16_t y, xpm_image_t img, uint8_t *map);

int (vg_move_xpm)(xpm_map_t xpm, uint16_t *xi, uint16_t *yi, uint16_t xf, uint16_t yf, uint16_t speed);

uint8_t (R)(uint32_t color);
uint8_t (G)(uint32_t color);
uint8_t (B)(uint32_t color);

/**
 * @brief Get horizontal resolution in pixels
 * 
 * @return horizontal resolution
*/
unsigned get_hres();

/**
 * @brief Get vertical resolution in pixels 
 * 
 * @return vertical resolution
*/
unsigned get_vres();

void (double_buffering)();

char* (get_double_buffer)();

int(load_all_xpms)();

#endif /* __VC_H */
