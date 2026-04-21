#ifndef ASM_H
#define ASM_H
#include <gb/gb.h>

/* engine */
extern void init_window_layer(void);
extern void lcd_isr(void);

uint8_t scroll_x_val = 0;

/* hud */
extern void update_score_hud(uint8_t score);
extern void write_score_label(void);

/* sram */
extern void save_score(uint8_t score);
extern uint8_t load_score(void);

/* text */
extern void draw_string(void);
extern void clear_string(void);
uint8_t text_x;
uint8_t text_y;
uint8_t text_len;
char *text_ptr;

/* vram */
extern void vram_copy(void);

uint8_t* asm_src;
uint8_t* asm_dest;
uint16_t asm_size;

#endif // !ASM_H
