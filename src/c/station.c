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

#define MENU_OPTIONS_LEN 3

#define MARKET_OPTION 0
#define DEPARTURE_OPTION 1
#define REPAIR_SHIP_OPTION 2

const char *menu_options[MENU_OPTIONS_LEN] = {
  "MARKET",
  "DEPARTURE",
  "REPAIR SHIP",
};

static uint8_t draw = 1;

void station_init(GameData* data) {
  data->state = STATION;
  move_win(7, 144);
  move_bkg(0, 0);
  clear_window();
  clear_bg();
  draw = 1;
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

  static uint8_t menu_cursor = 0;

  if (INPUT_PRESSED(PAD_UP)) {
    menu_cursor = (menu_cursor == 0) ? MENU_OPTIONS_LEN - 1 : menu_cursor - 1;
    draw = 1;
  }
  if (INPUT_PRESSED(PAD_DOWN)) {
    menu_cursor = (menu_cursor == MENU_OPTIONS_LEN - 1) ? 0 : menu_cursor + 1;
    draw = 1;
  }

  if (draw) {
    clear_lines_bg(0, 3);
    display_message_bg(0, 0, "STATION:");
    display_message_bg(8, 0, current_station->name);
    display_message_bg(0, 2, "CREDITS:");
    display_u16_bg(9, 2, save->credits);

    for(uint8_t i = 0; i < MENU_OPTIONS_LEN; i++) {
      if (menu_cursor == i) {
        display_message_bg(2, i + 4, "> ");
      } else {
        display_message_bg(2, i + 4, "  ");
      }
      display_message_bg(4, i + 4, menu_options[i]);
    }

    char buf[20];
    display_message_bg(0, 9, "SHIP:");
    display_message_bg(5, 9, save->player_ship.name);
    sprintf(buf, "HULL:%u/%u", save->player_ship.hp, save->player_ship.max_hp);
    display_message_bg(0, 10, buf);
    draw = 0;
  }

  if (INPUT_PRESSED(PAD_START)) {
    draw = 1;
    switch (menu_cursor) {
      case MARKET_OPTION:
        market_init(data);
        data->state = MARKET;
        break;

      case DEPARTURE_OPTION:
        if (save->player_ship.hp > 0) {
          dialog_start("Preparing warp...");
          data->state = TRAVEL;
        } else {
          dialog_start("Ship is too damaged to travel!");
        }
        break;

      case REPAIR_SHIP_OPTION:
        if (save->player_ship.hp < save->player_ship.max_hp && save->credits >= 50) {
          save->credits -= 50;
          save->player_ship.hp = save->player_ship.max_hp;
          clear_lines_bg(10, 11);
          dialog_start("Ship repaired!");
          sram_write(0, (uint8_t*)save, sizeof(SaveData));
        } else {
          dialog_start("Not enough credits!");
        }
        break;
    }
  }
}
