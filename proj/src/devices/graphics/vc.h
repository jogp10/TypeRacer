#ifndef __VC_H
#define __VC_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief 
 * 
 * @param mode change video graphics using VBE interface to mode
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (vc_change_mode)(uint16_t mode);

/**
 * @brief 
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (map_memory)();

/**
 * @brief 
 * 
 * @param mode get information about mode
 * @param info struct to return information on the mode
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (vc_get_mode_info)(uint16_t mode, vbe_mode_info_t *info);

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

int (vg_draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

int (vg_move_xpm)(xpm_map_t xpm, uint16_t *xi, uint16_t *yi, uint16_t xf, uint16_t yf, uint16_t speed);


#endif /* __VC_H */
