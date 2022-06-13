#ifndef __VC_H
#define __VC_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

/** @defgroup video_graphics video_graphics
 * @{
 *
 * video graphics module
 */

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


/**
 * @brief draw horizontal line
 * 
 * @param x position in x axis to draw
 * @param y position in y axis to draw
 * @param len leength of line
 * @param color color to drawn
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * @brief draw rectangle
 * 
 * @param x position in x axis to draw
 * @param y position in y axis to draw
 * @param width width of rectangle
 * @param height height of rectangle
 * @param color color to drawn
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

/**
 * @brief draw horizontal line
 * 
 * @param x position in x axis to draw
 * @param y position in y axis to draw
 * @param img xpm image
 * @param map map of colors of xpm
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (vg_draw_xpm)(uint16_t x, uint16_t y, xpm_image_t img, uint8_t *map);

/**
 * @brief draw horizontal line
 * 
 * @param xpm xpm to be draw
 * @param xi starting position in x axis to draw
 * @param yi starting position in y axis to draw
 * @param xf final position in x axis to draw
 * @param yf final position in y axis to draw
 * @param speed spped of the movement
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (vg_move_xpm)(xpm_map_t xpm, uint16_t *xi, uint16_t *yi, uint16_t xf, uint16_t yf, uint16_t speed);

/**
 * @brief masks red color into red position
 * 
 * @param color 
 */
uint8_t (R)(uint32_t color);

/**
 * @brief masks green color into green position
 * 
 * @param color 
 */
uint8_t (G)(uint32_t color);

/**
 * @brief masks blue color into blue position
 * 
 * @param color 
 */
uint8_t (B)(uint32_t color);

/**
 * @brief Get horizontal resolution in pixels
 * 
 * @return horizontal resolution
*/
int get_hres();

/**
 * @brief Get vertical resolution in pixels 
 * 
 * @return vertical resolution
*/
int get_vres();

/**
 * @brief double buffer setup
 */
void (double_buffering)();

/**
 * @brief
 * 
 * @return address to double buffer
 */
char* (get_double_buffer)();

/**
 * @brief Loads all game xpms
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int(load_all_xpms)();

/**
 * @brief clears xpm by drawing background
 * 
 * @param x starting position in x axis to draw
 * @param y starting position in y axis to draw
 * @param maxX final position in y axis to draw
 * @param maxY final position in x axis to draw
 * @param background_img xpm image to draw 
 * @param background_map color map of xpm
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int clear_xpm_with_cover(uint16_t x, uint16_t y, uint16_t maxX, uint16_t maxY, xpm_image_t background_img, uint8_t *background_map);

/**@}*/

#endif /* __VC_H */
