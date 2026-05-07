#ifndef ASM_WRAPPER_H
#define ASM_WRAPPER_H

#include <stdint.h>

void display_message(uint8_t x, uint8_t y, char* str);
void display_middle(uint8_t y, char *str);

void clear_message(uint8_t x, uint8_t y, uint8_t len);
void clear_line(uint8_t y);
void clear_window(void);

void update_score_display(uint8_t x, uint8_t y, uint16_t val);

uint8_t get_collision_at(uint8_t x, uint8_t y);

#endif // !ASM_WRAPPER_H
