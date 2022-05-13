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



#endif /* __VC_H */