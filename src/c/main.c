#include "asm.h"
#include "asm_wrapper.h"
#include "font.h"
#include "game.h"

const unsigned char player_tile[] = {
  0xFF,0xFF, 0x81,0x81, 0x81,0x81, 0x99, 0x99,
  0x99,0x99, 0x81, 0x81, 0x81, 0x81, 0xFF, 0xFF
};

const uint8_t my_map[360] = { 0 };

uint8_t* asm_src;
uint8_t* asm_dest;
uint16_t asm_size;

uint8_t scroll_x_val = 0;

void init_game(void) {
  asm_src = (uint8_t*)my_map;
  asm_dest = (uint8_t*)VRAM_ADDR;
  asm_size = 360;

  vram_copy();

  set_sprite_data(0, 1, player_tile);
  set_sprite_tile(0, 0);

  font_init();
  font_set(font_load(font_ibm));

  for(uint16_t i = START_WINDOW_MAP; i < END_WINDOW_MAP; i++) {
    while(STAT_REG & 0x02);
    *(uint8_t*)i = 0;
  }

  init_window_layer();

  move_win(7, 0);

  STAT_REG |= STATF_LYC;
  LYC_REG = 64;

  CRITICAL {
    add_LCD(lcd_isr);
  }
  set_interrupts(VBL_IFLAG | LCD_IFLAG);

  SPRITES_8x8;
  SHOW_SPRITES;
  SHOW_BKG;
  SHOW_WIN;
  DISPLAY_ON;
}

int main(void) {
  uint8_t keys_pressed;

  GameData data = {
    .state=TITLE,
    .player_x=80,
    .player_y=72,
    .best_score=load_score(),
    .joypad_current=joypad(),
    .joypad_previous=joypad(),
    .frame_counter=0,
  };

  init_game();

  while(1) {
    vsync();
    data.frame_counter++;

    data.joypad_previous = data.joypad_current;
    data.joypad_current = joypad();

    keys_pressed = data.joypad_current & ~data.joypad_previous;

    switch (data.state) {
      case TITLE:
        move_sprite(0, 0, 0);
        display_middle(9, "PRESS START");
        display_middle(10, "TO PLAY");

        if (keys_pressed & J_START) {
          clear_line(9);
          clear_line(10);
          data.player_x = 80;
          data.player_y = 72;
          scroll_x_val = 0;
          data.state = MINIGAME;
          display_message(0, 0, "SCORE:");
          update_score_display(6, 0, data.best_score);
        }
        break;

      case MINIGAME:
        if (data.joypad_current & J_UP && data.player_y > 16) data.player_y -= 2;
        if (data.joypad_current & J_DOWN && data.player_y < 152) data.player_y += 2;
        if (data.joypad_current & J_LEFT && data.player_x > 8) data.player_x -= 2;
        if (data.joypad_current & J_RIGHT && data.player_x < 160) data.player_x += 2;


        if (keys_pressed & J_A) {
          data.best_score++;
          save_score(data.best_score);
          update_score_display(6, 0, data.best_score);
        }

        move_sprite(0, data.player_x, data.player_y);

        if (keys_pressed & J_SELECT) {
          clear_message(0, 1, 11);
          data.state = GAMEOVER;
        }
        break;

      case GAMEOVER:
        move_sprite(0, 0, 0);
        display_middle(8, "GAME OVER");
        display_middle(10, "PRESS START");

        if (keys_pressed & J_START) {
          clear_window();
          data.state = TITLE;
        }
        break;
    }
  }
}
