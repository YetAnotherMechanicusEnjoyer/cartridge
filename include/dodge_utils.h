#ifndef DODGE_UTILS_H
#define DODGE_UTILS_H

#include <stdint.h>
#include "dodge_type.h"

uint8_t dodge_rand8(DodgeState* dodge);
uint8_t dodge_random_lane_x(DodgeState* dodge);
uint8_t dodge_collide8(uint8_t ax, uint8_t ay, uint8_t bx, uint8_t by);

void dodge_write_number_5(char* dst, uint16_t value);
void dodge_text_center_bg(uint8_t y, char* str);

#endif
