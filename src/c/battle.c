#include "battle.h"
#include "asm.h"
#include "dialog.h"
#include "input.h"
#include "asm_wrapper.h"
#include <gb/gb.h>
#include <stdio.h>

BattleManager bm;

extern void restore_overworld(void);

uint8_t calculate_damage(BattleEntity* attacker, BattleEntity* defender, Move* m) {
  if (m->power == 0) return 0;

  uint16_t lvl_factor = (2 * attacker->level / 5) + 2;
  uint32_t base = (uint32_t)lvl_factor * m->power * attacker->attack;

  uint32_t dmg = (base / defender->defense) / 50;

  return (uint8_t)dmg + 2;
}

void draw_battle_ui(void) {
  char buf[12];

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      display_message_bg(x, y, " ");
    }
  }

  display_message_bg(10, 1, bm.opponent->name);
  sprintf(buf, "HP:%d/%d", bm.opponent->hp, bm.opponent->max_hp);
  display_message_bg(10, 2, buf);

  display_message_bg(1, 8, bm.player->name);
  sprintf(buf, "HP:%d/%d", bm.player->hp, bm.player->max_hp);
  display_message_bg(1, 9, buf);
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
      dialog_start("A wild NPC appeared!");
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
      display_message(2, 2, bm.player->moves[1].name);

      display_message(1, 1, bm.menu_cursor == 0 ? ">" : " ");
      display_message(1, 2, bm.menu_cursor == 1 ? ">" : " ");

      if (INPUT_PRESSED(PAD_UP) || INPUT_PRESSED(PAD_DOWN)) {
        bm.menu_cursor = 1 - bm.menu_cursor;
      }

      if (INPUT_PRESSED(PAD_B)) {
        clear_message_win(1, 1, 18, 2);
        bm.state = B_MAIN_MENU;
      }

      if (INPUT_PRESSED(PAD_A)) {
        bm.player_move_idx = bm.menu_cursor;
        bm.enemy_move_idx = DIV_REG & 0x01;

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

      char atk_msg[30];
      sprintf(atk_msg, "%s uses %s!", attacker->name, used_move->name);
      dialog_start(atk_msg);

      uint8_t dmg = calculate_damage(attacker, defender, used_move);
      defender->hp = defender->hp <= dmg ? 0 : defender->hp - dmg;

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
      move_win(7, 144);

      restore_overworld();

      data->state = GAME;
      break;

    default: break;
  }
}
