#include "asm.h"
#include "audio.h"
#include "asm_wrapper.h"
#include "font.h"
#include "game.h"
#include "dialog.h"
#include <gb/gb.h>
#include "input.h"
#include "market.h"
#include "sram.h"
#include "trader.h"
#include <string.h>
#include <stdio.h>

#define MENU_OPTIONS_LEN 4

#define MARKET_OPTION 0
#define DEPARTURE_OPTION 1
#define REPAIR_SHIP_OPTION 2
#define SHIP_INFO_OPTION 3

#define S_MAIN 0
#define S_DETAILS 1

const char *menu_options[MENU_OPTIONS_LEN] = {
  "Market",
  "Departure",
  "Repair Ship",
  "Ship Info",
};

static uint8_t station_sub_state = S_MAIN;
static uint8_t draw = 1;
static uint8_t draw_cursor = 1;

static uint8_t cheat_progress = 0;

uint8_t any_input_pressed(void) {
  return INPUT_PRESSED(PAD_UP) || INPUT_PRESSED(PAD_DOWN) || INPUT_PRESSED(PAD_START) || INPUT_PRESSED(PAD_SELECT) || INPUT_PRESSED(PAD_RIGHT) || INPUT_PRESSED(PAD_LEFT) || INPUT_PRESSED(PAD_B) || INPUT_PRESSED(PAD_A);
}

void cheat_code(SaveData* save) {
    // Code : UP, UP, DOWN, DOWN, SELECT
    if (INPUT_PRESSED(PAD_UP) && (cheat_progress == 0 || cheat_progress == 1)) cheat_progress++;
    else if (INPUT_PRESSED(PAD_DOWN) && (cheat_progress == 2 || cheat_progress == 3)) cheat_progress++;
    else if (INPUT_PRESSED(PAD_SELECT) && cheat_progress == 4) {
        save->credits = 99999;
        sfx_warp();
        cheat_progress = 0;
    } else if (any_input_pressed()) {
        cheat_progress = 0;
    }
}

void snap_player_to_tile(GameData* data, uint8_t target_tile) {
    const uint8_t* current_map = station_registry[data->current_save->current_station_id].map;

    for (uint16_t i = 0; i < 1024; i++) {
        if (current_map[i] == target_tile) {
            uint8_t tile_x = i % 32;
            uint8_t tile_y = i / 32;

            data->player_x = tile_x * 8;
            data->player_y = tile_y * 8;

            int16_t cam_x = (int16_t)data->player_x - 80;
            int16_t cam_y = (int16_t)data->player_y - 72;

            if (cam_x < 0) cam_x = 0;
            if (cam_x > 96) cam_x = 96;
            if (cam_y < 0) cam_y = 0;
            if (cam_y > 112) cam_y = 112;

            apply_scroll_xy(cam_x, cam_y);

            break;
        }
    }
}

void station_init(GameData* data) {
  data->state = STATION;

  station_sub_state = S_MAIN;
  draw = 1;

  move_win(7, 144);
  move_bkg(0, 0);
  clear_window();
  clear_bg();
}

void station_state(GameData* data) {
  SaveData* save = data->current_save;
  uint8_t id = save->current_station_id;
  if (id >= MAX_STATIONS) id = 0;
  const StationDef* current_station = &station_registry[id];

  if (dialog_is_active()) {
    dialog_update();
    return;
  }

  if (save->player_ship.hp == 0 && save->credits < 50) {
    gameover_init(data);
    return;
  }

  cheat_code(save);

  static uint8_t menu_cursor = 0;
  char buf[22];

  if (station_sub_state == S_DETAILS) {
    if (draw) {
      clear_bg();

      display_message_bg(0, 2, "+==================+");
      display_message_bg(0, 3, "|   SHIP DATALOG   |");
      display_message_bg(0, 4, "+==================+");

      display_message_bg(2, 6, save->player_ship.name);

      sprintf(buf, "LVL:%u", save->player_ship.level);
      display_message_bg(2, 8, buf);

      sprintf(buf, "XP:%u", save->player_ship.xp);
      display_message_bg(11, 8, buf);

      sprintf(buf, "ATK:%u", save->player_ship.attack);
      display_message_bg(2, 9, buf);

      sprintf(buf, "DEF:%u", save->player_ship.defense);
      display_message_bg(11, 9, buf);

      sprintf(buf, "SPD:%u", save->player_ship.speed);
      display_message_bg(2, 10, buf);

      display_message_bg(0, 13, "+==================+");

      display_message_bg(4, 15, "[B] TO CLOSE");

      draw = 0;
    }

    if (INPUT_PRESSED(PAD_B)) {
      sfx_confirm();
      station_sub_state = S_MAIN;
      draw = 1;
    }
    return;
  }

  if (INPUT_PRESSED(PAD_UP)) {
    menu_cursor = (menu_cursor == 0) ? MENU_OPTIONS_LEN - 1 : menu_cursor - 1;
    draw_cursor = 1;
  }
  if (INPUT_PRESSED(PAD_DOWN)) {
    menu_cursor = (menu_cursor == MENU_OPTIONS_LEN - 1) ? 0 : menu_cursor + 1;
    draw_cursor = 1;
  }

  if (draw || draw_cursor) {
    for(uint8_t i = 0; i < MENU_OPTIONS_LEN; i++) {
      if (menu_cursor == i) {
        display_message_bg(2, i + 5, ">");
      } else {
        display_message_bg(2, i + 5, " ");
      }
      display_message_bg(4, i + 5, menu_options[i]);
    }
  }

  if (draw) {
    clear_bg();
    display_message_bg(0, 0, "====================");
    display_message_bg(5, 1, current_station->name);
    display_message_bg(0, 2, "====================");

    sprintf(buf, "%u", save->credits);
    display_message_bg(16 - strlen(buf), 3, buf);
    display_message_bg(17, 3, "CR");

    display_message_bg(0, 13, "--------------------");
    display_message_bg(1, 14, "SHIP:");
    display_message_bg(6, 14, save->player_ship.name);
    sprintf(buf, "HULL: %u/%u", save->player_ship.hp, save->player_ship.max_hp);
    display_message_bg(1, 16, buf);
    draw = 0;
  }

  if (INPUT_PRESSED(PAD_A)) {
    sfx_confirm();
    draw = 1;
    switch (menu_cursor) {
      case MARKET_OPTION:
        snap_player_to_tile(data, 4);
        market_init(data);
        data->state = MARKET;
        break;

      case DEPARTURE_OPTION:
        if (save->player_ship.hp > 0) {
          travel_init(data);
        } else {
          dialog_start("Ship is too damaged to travel!");
        }
        break;

      case REPAIR_SHIP_OPTION:
        if (save->player_ship.hp < save->player_ship.max_hp) {
          if (save->credits >= 50) {
            save->credits -= 50;
            save->player_ship.hp = save->player_ship.max_hp;
            clear_lines_bg(10, 11);
            dialog_start("Ship repaired!");
            sram_write(0, (uint8_t*)save, sizeof(SaveData));
          } else dialog_start("Not enough credits!");
        } else dialog_start("Ship is not damaged!");
        break;

      case SHIP_INFO_OPTION:
        station_sub_state = S_DETAILS;
        break;
    }
  }
}
