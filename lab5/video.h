#ifndef __VIDEO_H
#define __VIDEO_H

#include <stdbool.h>
#include <stdint.h>
#include <lcom/lcf.h>
#include "minix/sysutil.h"

#define OK 0
#define RETURN_CONTROLLER 0x00
#define RETURN_MODE 0x01
#define SET_MODE 0x02


/**
 * @brief Sets graphics mode to mode specified by user
 * 
 */
int (video_set_graphics_mode)(uint16_t mode);


/**
 * @brief Maps Video RAM in a Process's Address Space 
 * 
 */
int(video_map_addr)(endpoint_t who, void *phaddr, size_t len);


#endif /* __VIDEO_H */