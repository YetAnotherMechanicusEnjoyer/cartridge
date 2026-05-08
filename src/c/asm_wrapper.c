#include "asm_wrapper.h"
#include "asm.h"
#include <stdint.h>

uint8_t text_x;
uint8_t text_y;
uint8_t text_len;

char *text_ptr;

uint16_t text_val;

uint8_t hitbox_x;
uint8_t hitbox_y;

static uint8_t getlen(const char* str) {
    uint8_t i = 0;

    if (!str)
        return i;
    for (; str[i]; i++);
    return i;
}

void display_message(uint8_t x, uint8_t y, char* str) {
  text_x = x;
  text_y = y;
  text_ptr = str;
  draw_string();
}

void display_middle(uint8_t y, char *str) {
  uint8_t len = getlen(str);
  if (len > WIDTH) len = WIDTH;
  uint8_t middle = (uint8_t)(WIDTH - len) / (uint8_t)2;
  text_x = middle;
  text_y = y;
  text_ptr = str;
  draw_string();
}

void display_message_bg(uint8_t x, uint8_t y, char *str) {
  uint8_t len = getlen(str);
  if (len == 0 || len > 20) return;

  uint8_t tiles[20];

  for (uint8_t i = 0; i < len; i++) {
    tiles[i] = str[i] - 32;
  }

  set_bkg_tiles(x, y, len, 1, tiles);
}

void draw_menu_border_win(void) {
  display_message(0, 0, "+------------------+");
  display_message(0, 1, "+                  +");
  display_message(0, 2, "+                  +");
  display_message(0, 3, "+------------------+");
}

void clear_message(uint8_t x, uint8_t y, uint8_t len) {
  text_x = x;
  text_y = y;
  text_len = len;
  clear_string();
}

void clear_line(uint8_t y) {
  text_x = 0;
  text_y = y;
  text_len = WIDTH;
  clear_string();
}

void clear_window(void) {
  for(uint8_t y = 0; y < HEIGHT; y++) {
    clear_message(0, y, WIDTH);
  }
}

void clear_message_win(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  for (uint8_t i = 0; i < h; i++) {
    clear_message(x, y + i, w);
  }
}

void update_score_display(uint8_t x, uint8_t y, uint16_t val) {
  char buf[7];
  text_val = val;
  text_ptr = buf;
  utoa();
  display_message(x, y, buf);
}

uint8_t get_collision_at(uint8_t x, uint8_t y) {
  hitbox_x = x;
  hitbox_y = y;

  return get_hitbox_value();
}
