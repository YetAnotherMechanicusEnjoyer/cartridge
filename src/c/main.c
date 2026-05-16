#include "asm.h"
#include "asm_wrapper.h"
#include "audio.h"
#include "battle.h"
#include "battle_func.h"
#include "dialog.h"
#include "market.h"
#include "sprites.h"
#include "font.h"
#include "dodge_blocks.h"
#include "game.h"
#include "gb/gb.h"
#include "input.h"
#include "sram.h"
#include "timers.h"

uint8_t* asm_src;
uint8_t* asm_dest;
uint16_t asm_size;

void restore_overworld(void) {
  //set_bkg_data(128, background_TILE_COUNT, background_tiles);
  //set_bkg_based_tiles(0, 0, background_WIDTH / 8, background_HEIGHT / 8, background_map, 128);

  SHOW_SPRITES;
}

static void void_init(GameData *data) { data->state = TITLE; }

static void init_game(void) {
  font_init();
  font_set(font_load(font_ibm));

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

  init_sound();

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
    { .name="Station", .init=station_init, .game=station_state },
    { .name="Dodge Blocks", .init=dodge_blocks_init, .game=dodge_blocks_game},
  };

  uint8_t len = sizeof(registry) / sizeof(registry[0]);

  /*
  uint8_t save_initialized;
  uint32_t credits;
  uint8_t current_station_id;
  uint8_t market_seed;
  InvSlot inventory[MAX_INVENTORY_SLOTS];
  BattleEntity player_ship;

  uint8_t item_id;
  uint8_t quantity;
   */

  SaveData current_save;
  current_save.save_initialized = 0;

  sram_read(0, (uint8_t*)&current_save, sizeof(SaveData));

  if (current_save.save_initialized != SAVE_INITIALIZED) {
    SaveData new_save = {
      .save_initialized=SAVE_INITIALIZED,
      .credits=100,
      .current_station_id=0,
      .market_seed=42,
      .inventory={
        {42, 0},
        {42, 0},
        {42, 0},
        {42, 0},
        {42, 0},
        {42, 0},
        {42, 0},
        {42, 0},
      },
      .player_ship={
        .name="Cutlass Black",
        .level=5,
        .xp=0,
        .hp=20,
        .max_hp=20,
        .attack=10,
        .defense=8,
        .speed=10,
        .type1=T_FIRE,
        .type2=T_NORMAL,
        .moves={
          { .name="TEMPEST II", .power=40, .type=T_NORMAL, .effect=EFF_DAMAGE },
          { .name="CF-337", .power=40, .type=T_FIRE, .effect=EFF_DAMAGE },
          { .name="ARRESTER I", .power=0, .type=T_NORMAL, .effect=EFF_DEFENSE_DOWN },
          { .name="GT-220", .power=40, .type=T_NORMAL, .effect=EFF_DAMAGE },
        }
      }
    };
    current_save = new_save;
    sram_write(0, (uint8_t*)&current_save, sizeof(SaveData));
  }

  GameData data = {
    .state=TITLE,
    .player_x=80,
    .player_y=72,
    .score=0,
    .current_save=&current_save,
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
    if (dialog_is_active()) {
      dialog_update();
      continue;
    }


    switch (data.state) {
      case TITLE:
        HIDE_SPRITES;
        move_win(7, 0);
        title_state(&data);
        break;
      case GAME:
        restore_overworld();
        data.games[data.current_game_id].game(&data);
        break;
      case STATION:
        station_state(&data);
        break;
      case MARKET:
        market_state(&data);
        break;
      case TRAVEL:
        travel_state(&data);
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
