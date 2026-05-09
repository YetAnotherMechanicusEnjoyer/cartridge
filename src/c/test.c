#include "asm.h"
#include "asm_wrapper.h"
#include "battle.h"
#include "battle_func.h"
#include "dialog.h"
#include "npc.h"
#include "game.h"
#include "gb/gb.h"
#include "input.h"
#include "sram.h"
#include "timers.h"
#include <stdint.h>

#define TIMER_INVICIBLE 0

#define FEET_OFFSET_X 4
#define FEET_OFFSET_Y 14

#define EMPTY 0
#define WALL 1
#define TALL_GRASS 2

uint8_t col_x = 0;
uint8_t col_y = 0;
uint8_t scroll_x = 0;
uint8_t scroll_y = 0;

BattleEntity charmander_stats = {
  "Charmander", 5, 20, 20,
  10, 10, 15, 0,
  T_FIRE, T_NORMAL,
  {
    { "TACKLE", 40, T_NORMAL, EFF_DAMAGE },
    { "TAIL WHIP", 0, T_NORMAL, EFF_DEFENSE_DOWN },
    { "EMBER", 40, T_FIRE, EFF_DAMAGE },
    { "", 0, T_NORMAL, EFF_DAMAGE }
  }
};

BattleEntity bulbasaur_stats = {
  "Bulbasaur", 5, 20, 20,
  9, 9, 13, 0,
  T_GRASS, T_NORMAL,
  {
    { "TACKLE", 40, T_NORMAL, EFF_DAMAGE },
    { "GROWL", 0, T_NORMAL, EFF_ATTACK_UP },
    { "", 0, T_NORMAL, EFF_DAMAGE },
    { "", 0, T_NORMAL, EFF_DAMAGE },
  }
};

const uint8_t collision_map[1024] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
};

void test_init(GameData* data) {
  data->player_x = 25;
  data->player_y = 20;
  data->score = 0;
  npcs_init();

  npcs[0].x = 80;
  npcs[0].y = 60;
  npcs[0].sprite_id = 4;
  npcs[0].active = 1;
  npcs[0].moving = 0;
  npcs[0].wait_timer = 30;

  move_win(7, 144);
  /*
  display_message(0, 0, "SCORE:");
  update_score_display(6, 0, data->current_save.high_score);
  */
}

static void test_movement(GameData* data) {
  uint8_t next_x = data->player_x;
  uint8_t next_y = data->player_y;

  if (INPUT_HELD(PAD_UP) && data->player_y > 0) next_y -= 1;
  if (INPUT_HELD(PAD_DOWN) && data->player_y < 255) next_y += 1;
  if (INPUT_HELD(PAD_LEFT) && data->player_x > 0) next_x -= 1;
  if (INPUT_HELD(PAD_RIGHT) && data->player_x < 255) next_x += 1;

  uint8_t terrain_type = get_collision_at(next_x + FEET_OFFSET_X, next_y + FEET_OFFSET_Y);

  if (terrain_type != WALL) {
    data->player_x = next_x;
    data->player_y = next_y;
  }

  switch (terrain_type) {
    case TALL_GRASS:
      if ((DIV_REG & 0xFF) < 5) {
        fade_out_black();
        battle_init(data, &data->current_save.player_bentity, &bulbasaur_stats);
        fade_in_black();
        return;
      }
  }

  int16_t cam_x = data->player_x;
  int16_t cam_y = data->player_y;

  if (cam_x <= 80) cam_x = 0;
  else if (cam_x - 80 > 96) cam_x = 96;
  else cam_x -= 80;

  if (cam_y <= 72) cam_y = 0;
  else if (cam_y - 72 > 112) cam_y = 112;
  else cam_y -= 72;

  scroll_x = cam_x;
  scroll_y = cam_y;
  apply_scroll();

  uint8_t sprite_x = (data->player_x - scroll_x) + 8;
  uint8_t sprite_y = (data->player_y - scroll_y) + 16;

  move_sprite(0, sprite_x, sprite_y);
  set_sprite_tile(0, 0);

  npcs_render(scroll_x, scroll_y);
}

static void test_events(GameData* data) {
  if (dialog_is_active()) {
    dialog_update();
    return;
  }

  test_movement(data);

  if (INPUT_PRESSED(PAD_B)) {
    dialog_start("Hi strange player!");
    dialog_start("Do what you want, just don't bother me.");
    dialog_start("I'm heading back to work... again...");
    dialog_start("Wait I forgot, it's not licensed by Nintendo hihi");
    dialog_start("Cya !");
  }

  /*
  if (INPUT_PRESSED(PAD_A)) {
    if (timer_counters[TIMER_INVICIBLE] == 0) {
      set_timer(TIMER_INVICIBLE, 60);
      uint16_t new_score = data->score + 10;
      if (new_score < data->score) {
        data->state = GAMEOVER;
        return;
      }
      data->score = new_score;
      move_win(7, 112);
      update_score_display(6, 0, data->score);
      display_message(17, 0, "...");
    } else {
      display_message(0, 1, "/!\\ Wait /!\\");
    }
  }
  */

  if (INPUT_RELEASED(PAD_SELECT)) {
    if (data->current_save.high_score < data->score) {
      data->current_save.high_score = data->score;
      sram_write(0, (uint8_t*)&data->current_save, sizeof(SaveData));
    }
    gameover_init(data);
    data->state = GAMEOVER;
  }
}

void test_game(GameData* data) {
  /*
  if (check_timer(TIMER_INVICIBLE)) {
    move_win(7, 144);
    clear_message(17, 0, 3);
    clear_line(1);
  }
  */
  test_events(data);

  if (data->current_save.player_bentity.hp == 0) {
    data->current_save.player_bentity.hp = data->current_save.player_bentity.max_hp;
    data->current_save.save_initialized = SAVE_INITIALIZED;
    sram_write(0, (uint8_t*)&data->current_save, sizeof(SaveData));
    gameover_init(data);
    data->state = GAMEOVER;
  }

  for (uint8_t i = 0; i < MAX_NPC; i++) {
    npc_process_logic(&npcs[i]);

    if (npcs[i].active) {
      move_sprite(i + 1, (npcs[i].x - scroll_x) + 8, (npcs[i].y - scroll_y) + 16);
      set_sprite_tile(i + 1, npcs[i].sprite_id);
    }
  }
}
