#include "asm.h"
#include "battle.h"
#include "battle_func.h"
#include "dialog.h"
#include "input.h"
#include "asm_wrapper.h"
#include "sram.h"
#include "ui_assets.h"
#include <gb/gb.h>
#include <stdio.h>

#define HP_BAR_LENGTH 6

#define END_OF_CHART 0xFF

BattleManager bm;

const uint8_t type_chart[] = {
  T_FIRE, T_GRASS, 20,
  T_FIRE, T_WATER, 5,
  T_WATER, T_FIRE, 20,
  T_WATER, T_GRASS, 5,
  T_GRASS, T_WATER, 20,
  T_GRASS, T_FIRE, 5,
  END_OF_CHART
};

uint8_t get_type_modifier(Type atk_type, Type def_type) {
  uint8_t i = 0;
  while (type_chart[i] != END_OF_CHART) {
    if (type_chart[i] == atk_type && type_chart[i + 1] == def_type) {
      return type_chart[i + 2];
    }
    i += 3;
  }
  return 10;
}

extern void restore_overworld(void);

void give_xp(BattleEntity* player, BattleEntity* enemy) {
  char msg[40];

  uint16_t xp_gained = (50 * enemy->level) / 7;

  sprintf(msg, "%s gained %d XP!", player->name, xp_gained);
  dialog_start(msg);

  player->xp += xp_gained;

  uint16_t xp_needed = (uint16_t)player->level * player->level * 10;

  if (player->xp >= xp_needed) {
    player->level++;

    player->max_hp += 2;
    player->hp = player->max_hp;
    player->attack += 1;
    player->defense += 1;
    player->speed += 1;

    sprintf(msg, "%s grew to level %d!", player->name, player->level);
    dialog_start(msg);
  }
}

uint8_t calculate_damage(BattleEntity* attacker, BattleEntity* defender, Move* m) {
  if (m->power == 0 || m->effect != EFF_DAMAGE) return 0;

  uint16_t lvl_factor = (2 * attacker->level / 5) + 2;
  uint32_t base = (uint32_t)lvl_factor * m->power * attacker->attack;

  uint32_t dmg = ((base / defender->defense) / 50) + 2;

  if (m->type == attacker->type1 || m->type == attacker->type2) {
    dmg = (dmg * 15) / 10;
  }

  uint8_t mod1 = get_type_modifier(m->type, defender->type1);
  uint8_t mod2 = get_type_modifier(m->type, defender->type2);

  dmg = (dmg * mod1 * mod2) / 100;

  return (uint8_t)dmg;
}

void draw_graphical_hp_bar(uint8_t x, uint8_t y, uint16_t hp, uint16_t max_hp) {
  if (max_hp == 0) return;

  uint8_t tiles_to_draw[HP_BAR_LENGTH];

  uint16_t total_pixels = ((uint32_t)hp * (HP_BAR_LENGTH * 8)) / max_hp;

  uint8_t full_tiles = total_pixels / 8;
  uint8_t remainder = total_pixels % 8;

  for (uint8_t i = 0; i < HP_BAR_LENGTH; i++) {
    if (i < full_tiles) {
      tiles_to_draw[i] = HP_TILES_OFFSET + 8;
    } else if (i == full_tiles) {
      tiles_to_draw[i] = HP_TILES_OFFSET + remainder;
    } else {
      tiles_to_draw[i] = HP_TILES_OFFSET + 0;
    }
  }

  set_bkg_tiles(x, y, HP_BAR_LENGTH, 1, tiles_to_draw);
}

void animate_damage(BattleEntity* target, uint8_t damage, uint8_t bar_x, uint8_t bar_y) {
  for (uint8_t i = 0; i < damage; i++) {
    if (target->hp == 0) break;

    target->hp--;

    draw_graphical_hp_bar(bar_x, bar_y, target->hp, target->max_hp);

    vsync();
    vsync();
  }
}

void draw_battle_ui(void) {
  char buf[12];

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      display_message_bg(x, y, " ");
    }
  }

  sprintf(buf, "Level %d", bm.opponent->level);
  display_message_bg(10, 0, buf);
  display_message_bg(10, 1, bm.opponent->name);
  sprintf(buf, "HP:%d/%d", bm.opponent->hp, bm.opponent->max_hp);
  display_message_bg(10, 2, buf);
  draw_graphical_hp_bar(10, 3, bm.opponent->hp, bm.opponent->max_hp);

  sprintf(buf, "Level %d", bm.player->level);
  display_message_bg(1, 7, buf);
  display_message_bg(1, 8, bm.player->name);
  sprintf(buf, "HP:%d/%d", bm.player->hp, bm.player->max_hp);
  display_message_bg(1, 9, buf);
  draw_graphical_hp_bar(1, 10, bm.player->hp, bm.player->max_hp);
}

void set_window_mode(uint8_t is_menu) {
  clear_message_win(0, 0, 20, 4);

  if (is_menu) draw_menu_border_win();

  move_win(7, 112);
}

void battle_init(GameData* data, BattleEntity* p, BattleEntity* o) {
  HIDE_SPRITES;

  bm.player = p;
  bm.opponent = o;
  bm.state = B_INIT;

  data->state = BATTLE;

  bm.is_player_faster = (p->speed >= o->speed);

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      display_message_bg(x, y, " ");
    }
  }

  draw_battle_ui();
  set_bkg_data(HP_TILES_OFFSET, 9, hp_bar_tiles);
  move_win(7, 112);
}

void battle_update(GameData *data) {
  if (dialog_is_active()) {
    dialog_update();
    return;
  }

  uint8_t dmg = 0;

  switch (bm.state) {
    case B_INIT:
      if (bm.player->hp == 0 || bm.opponent->hp == 0) {
        bm.state = B_CHECK_DEATH;
        break;
      }
      char enemy_trigger_msg[40];
      sprintf(enemy_trigger_msg, "A wild %s appeared!", bm.opponent->name);
      dialog_start(enemy_trigger_msg);
      dialog_start("What will you do?");
      bm.state = B_WAIT_INTRO;
      break;

    case B_WAIT_INTRO:
      bm.state = B_MAIN_MENU;
      bm.menu_cursor = 0;
      bm.turn_step = 0;
      set_window_mode(1);
      break;

    case B_MAIN_MENU:
      display_message(2, 1, "FIGHT");
      display_message(10, 1, "RUN");

      display_message(1, 1, bm.menu_cursor == 0 ? ">" : " ");
      display_message(9, 1, bm.menu_cursor == 1 ? ">" : " ");

      if (INPUT_PRESSED(PAD_LEFT) || INPUT_PRESSED(PAD_RIGHT)) {
        bm.menu_cursor = 1 - bm.menu_cursor;
      }

      if (INPUT_PRESSED(PAD_A)) {
        if (bm.menu_cursor == 0) {
          set_window_mode(1);
          bm.state = B_MOVE_MENU;
          bm.menu_cursor = 0;
        } else {
          set_window_mode(0);
          dialog_start("Got away safely!");
          bm.state = B_END_BATTLE;
        }
      }
      break;

    case B_MOVE_MENU:
      display_message(2, 1, bm.player->moves[0].name);
      display_message(11, 1, bm.player->moves[1].name);
      display_message(2, 2, bm.player->moves[2].name);
      display_message(11, 2, bm.player->moves[3].name);

      display_message(1, 1, bm.menu_cursor == 0 ? ">" : " ");
      display_message(10, 1, bm.menu_cursor == 1 ? ">" : " ");
      display_message(1, 2, bm.menu_cursor == 2 ? ">" : " ");
      display_message(10, 2, bm.menu_cursor == 3 ? ">" : " ");

      if (INPUT_PRESSED(PAD_LEFT) || INPUT_PRESSED(PAD_RIGHT)) {
        bm.menu_cursor ^= 1;
      }

      if (INPUT_PRESSED(PAD_UP) || INPUT_PRESSED(PAD_DOWN)) {
        bm.menu_cursor ^= 2;
      }

      if (INPUT_PRESSED(PAD_B)) {
        set_window_mode(1);
        bm.state = B_MAIN_MENU;
        bm.menu_cursor = 0;
      }

      if (INPUT_PRESSED(PAD_A) && bm.player->moves[bm.menu_cursor].name[0] != '\0') {
        bm.player_move_idx = bm.menu_cursor;

        uint8_t random_enemy_move;
        do {
          random_enemy_move = DIV_REG & 0x03;
        } while (bm.opponent->moves[random_enemy_move].name[0] == '\0');

        bm.enemy_move_idx = random_enemy_move;

        set_window_mode(0);
        bm.state = B_SPEED_CHECK;
      }
      break;

    case B_SPEED_CHECK:
      if (bm.player->speed >= bm.opponent->speed) {
        bm.first_attacker = ID_PLAYER;
      } else {
        bm.first_attacker = ID_OPPONENT;
      }
      bm.current_attacker = bm.first_attacker;
      bm.turn_step = 0;
      bm.state = B_EXECUTE_TURN;
      break;

    case B_EXECUTE_TURN: {
      set_window_mode(0);
      BattleEntity* attacker = (bm.current_attacker == ID_PLAYER) ? bm.player : bm.opponent;
      BattleEntity* defender = (bm.current_attacker == ID_PLAYER) ? bm.opponent : bm.player;

      uint8_t move_idx = (bm.current_attacker == ID_PLAYER) ? bm.player_move_idx : bm.enemy_move_idx;
      Move* used_move = &attacker->moves[move_idx];

      char atk_msg[40];
      sprintf(atk_msg, "%s uses %s!", attacker->name, used_move->name);
      dialog_start(atk_msg);

      switch (used_move->effect) {
        case EFF_DAMAGE:
          uint8_t dmg = calculate_damage(attacker, defender, used_move);

          uint8_t bar_x = (bm.current_attacker == ID_PLAYER) ? 10 : 1;
          uint8_t bar_y = (bm.current_attacker == ID_PLAYER) ? 3 : 10;
          animate_damage(defender, dmg, bar_x, bar_y);

          uint8_t mod = get_type_modifier(used_move->type, defender->type1);
          if (mod == 20) dialog_start("It's super effective!");
          else if (mod == 5) dialog_start("It's not very effective...");
          else if (mod == 0) dialog_start("It had no effect!");
          break;

        case EFF_DEFENSE_DOWN:
          if (defender->defense > 3) {
            defender->defense -= 2;
            char def_down_msg[40];
            sprintf(def_down_msg, "%s's defense fell!", defender->name);
            dialog_start(def_down_msg);
          } else {
            dialog_start("Defense won't go any lower!");
          }
          break;

        case EFF_DEFENSE_UP:
          if (attacker->defense < 29) {
            attacker->defense += 2;
            char def_up_msg[40];
            sprintf(def_up_msg, "%s's defense rose!", attacker->name);
            dialog_start(def_up_msg);
          } else {
            dialog_start("Defense won't go any higher!");
          }
          break;

        case EFF_ATTACK_DOWN:
          if (defender->attack > 3) {
            defender->attack -= 2;
            char atk_down_msg[40];
            sprintf(atk_down_msg, "%s's attack fell!", defender->name);
            dialog_start(atk_down_msg);
          } else {
            dialog_start("Attack won't go any lower!");
          }
          break;

        case EFF_ATTACK_UP:
          if (attacker->attack < 29) {
            attacker->attack += 2;
            char atk_up_msg[40];
            sprintf(atk_up_msg, "%s's attack rose!", attacker->name);
            dialog_start(atk_up_msg);
          } else {
            dialog_start("Attack won't go any higher!");
          }
          break;
      }

      draw_battle_ui();
      bm.state = B_CHECK_DEATH;
      break;
    }

    case B_CHECK_DEATH: {
      BattleEntity* defender = (bm.current_attacker == ID_PLAYER) ? bm.opponent : bm.player;

      if (defender->hp <= 0) {
        char faint_msg[20];
        sprintf(faint_msg, "%s fainted!", defender->name);
        dialog_start(faint_msg);

        if (bm.current_attacker == ID_PLAYER) {
          give_xp(bm.player, bm.opponent);
          sram_write(0, (uint8_t*)&data->current_save, sizeof(SaveData));
        } else {
          dialog_start("You blacked out...");
        }
        bm.state = B_END_BATTLE;
      } else {
        if (bm.turn_step == 0) {
          bm.turn_step = 1;
          bm.current_attacker = (bm.first_attacker == ID_PLAYER) ? ID_OPPONENT : ID_PLAYER;
          bm.state = B_EXECUTE_TURN;
        } else {
          bm.state = B_MAIN_MENU;
          bm.menu_cursor = 0;
          set_window_mode(1);
        }
      }
      break;
    }

    case B_END_BATTLE:
      fade_out_black();

      move_win(7, 144);
      data->state = STATION;

      fade_in_black();
      break;

    default: break;
  }
}
