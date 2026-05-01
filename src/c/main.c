#include "asm.h"
#include "font.h"
#include "game.h"
#include "input.h"
#include "timers.h"

const unsigned char player_tile[] = {
  0xFF,0xFF, 0x81,0x81, 0x81,0x81, 0x99, 0x99,
  0x99,0x99, 0x81, 0x81, 0x81, 0x81, 0xFF, 0xFF
};

const uint8_t my_map[360] = { 0 };

uint8_t* asm_src;
uint8_t* asm_dest;
uint16_t asm_size;

static void void_init(GameData *data) { data->state = TITLE; }

static void init_game(void) {
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
  Game registry[] = {
    { .name="Test Game", .init=test_init, .game=test_game },
    { .name="Not a Pong", .init=void_init, .game=title_state },
    { .name="Testing (again)", .init=void_init, .game=title_state },
    { .name="Menu", .init=void_init, .game=title_state },
  };

  uint8_t len = sizeof(registry) / sizeof(registry[0]);

  GameData data = {
    .state=TITLE,
    .player_x=80,
    .player_y=72,
    .best_score=0,
    .frame_counter=0,
    .current_game_id=0,
    .n_games=len,
    .games=registry,
  };

  init_game();

  while(1) {
    vsync();
    timers_update();
    input_update();

    display_message(0, HEIGHT - 2, "p:");
    update_score_display(2, HEIGHT - 2, pad_previous);
    display_message(0, HEIGHT - 1, "c:");
    update_score_display(2, HEIGHT - 1, pad_current);

    data.frame_counter++;

    switch (data.state) {
      case TITLE:
        title_state(&data);
        break;
      case GAME:
        data.games[data.current_game_id].game(&data);
        break;
      case GAMEOVER:
        gameover_state(&data);
        break;
    }
  }
}
