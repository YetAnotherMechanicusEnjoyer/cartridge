#ifndef SRAM_H
#define SRAM_H

#include <stdint.h>

#define SAVE_INITIALIZED (uint8_t)42

extern void sram_write(uint16_t offset, uint8_t* data, uint16_t length) __sdcccall(0);
extern void sram_read(uint16_t offset, uint8_t* data, uint16_t length) __sdcccall(0);

#endif // !SRAM_H
