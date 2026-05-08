#include "asm.h"
#include "asm_wrapper.h"
#include "battle.h"
#include "sprites.h"
#include "font.h"
#include "game.h"
#include "gb/gb.h"
#include "input.h"
#include "sram.h"
#include "timers.h"

uint8_t* asm_src;
uint8_t* asm_dest;
uint16_t asm_size;

#define BKG_TILES_OFFSET 128

void restore_overworld(void) {
  set_bkg_data(BKG_TILES_OFFSET, background_TILE_COUNT, background_tiles);
  set_bkg_based_tiles(0, 0, background_WIDTH / 8, background_HEIGHT / 8, background_map, BKG_TILES_OFFSET);

  SHOW_SPRITES;
}

static void void_init(GameData *data) { data->state = TITLE; }

static void init_game(void) {
  font_init();
  font_set(font_load(font_ibm));

  restore_overworld();

  //asm_src = (uint8_t*)background_tiles;
  //asm_dest = (uint8_t*)VRAM_ADDR;
  //asm_size = 360;

  //vram_copy();

  set_sprite_data(0, player_TILE_COUNT, player_tiles);
  set_sprite_tile(0, 0);

  set_sprite_data(4, npc1_TILE_COUNT, npc1_tiles);

  for(uint16_t i = START_WINDOW_MAP; i < END_WINDOW_MAP; i++) {
    while(STAT_REG & 0x02);
    *(uint8_t*)i = 0;
  }

  init_window_layer();

  STAT_REG |= STATF_LYC;
  LYC_REG = 64;

  CRITICAL {
    add_LCD(lcd_isr);
  }
  set_interrupts(VBL_IFLAG);

  BGP_REG = 0xE4;
  OBP0_REG = 0xE4;

  SPRITES_8x8;
  SHOW_BKG;
  SHOW_WIN;
  DISPLAY_ON;
}

void update(GameData* data) {
  vsync();
  timers_update();
  input_update();

  data->frame_counter++;
}

int main(void) {
  Game registry[] = {
    { .name="Test Game", .init=test_init, .game=test_game },
    { .name="Not a Pong", .init=void_init, .game=title_state },
    { .name="Testing (again)", .init=void_init, .game=title_state },
    { .name="Menu", .init=void_init, .game=title_state },
  };

  uint8_t len = sizeof(registry) / sizeof(registry[0]);

  SaveData current_save = {
    .save_initialized=0,
    .high_score=0,
  };

  sram_read(0, (uint8_t*)&current_save, sizeof(SaveData));

  if (current_save.save_initialized != SAVE_INITIALIZED) {
    current_save.save_initialized = SAVE_INITIALIZED;
    sram_write(0, (uint8_t*)&current_save, sizeof(SaveData));
  }

  GameData data = {
    .state=TITLE,
    .player_x=80,
    .player_y=72,
    .score=0,
    .current_save=current_save,
    .frame_counter=0,
    .current_game_id=0,
    .n_games=len,
    .games=registry,
  };

  init_game();

  while(1) {
    update(&data);
    /*
    if (data.frame_counter % 10 == 0) {
      uint8_t height = data.state == GAME ? 2 : HEIGHT - 1;
      display_message(0, height, "c:");
      update_score_display(2, height, pad_current);
    }
    */

    switch (data.state) {
      case TITLE:
        HIDE_SPRITES;
        move_win(7, 0);
        title_state(&data);
        break;
      case GAME:
        SHOW_SPRITES;
        data.games[data.current_game_id].game(&data);
        break;
      case BATTLE:
        battle_update(&data);
        break;
      case GAMEOVER:
        HIDE_SPRITES;
        gameover_state(&data);
        break;
    }
  }
}
