#include <stdint.h>
#include "utils.h"

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb==NULL) return 1;
  *lsb = (uint8_t) val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb==NULL) return 1;
  *msb = (uint8_t) (val >> 8);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t var;

  if(sys_inb(port, &var)) {
    printf("Error reading byte\n");
    return 1;
  };

  *value = var & 0xFF;
  return 0;
}
