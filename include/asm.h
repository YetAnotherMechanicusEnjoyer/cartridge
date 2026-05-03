#ifndef ASM_H
#define ASM_H

#include <gb/gb.h>

#define WIDTH 20
#define HEIGHT 18

#define START_WINDOW_MAP 0x9C00
#define END_WINDOW_MAP 0x9FFF

#define VRAM_ADDR 0x9800

/*
  === FUNCTIONS ===
*/

/* collision */
extern uint8_t col_x;
extern uint8_t col_y;

extern uint8_t check_collision(void);

/* engine */
extern void init_window_layer(void);
extern void lcd_isr(void);

/* sram */
extern void save_score(uint16_t score);
extern uint16_t load_score(void);

/* text */
extern void draw_string(void);
extern void clear_string(void);

/* utoa */
extern void utoa(void);

/* vram */
extern void vram_copy(void);

#endif // !ASM_H
