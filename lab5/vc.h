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

int (map_memory)();

int (vc_get_mode_info)(uint16_t mode, vbe_mode_info_t *info);



#endif /* __VC_H */
