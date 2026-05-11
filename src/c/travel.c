#include "asm.h"
#include "asm_wrapper.h"
#include "battle_func.h"
#include "encounter.h"
#include "game.h"
#include "dialog.h"
#include "input.h"
#include "sram.h"
#include "trader.h"
#include <stdio.h>
#include <string.h>

BattleEntity wild_enemy;

static uint8_t travel_draw_full = 1;
static uint8_t travel_draw_info = 1;

static uint8_t travel_warp = 0;

void travel_init(GameData* data) {
  data->state = TRAVEL;

  travel_draw_full = 1;
  travel_draw_info = 1;
  travel_warp = 0;

  move_win(7, 144);
  move_bkg(0, 0);
  clear_window();
  clear_bg();
}

void travel_state(GameData* data) {
  SaveData* save = data->current_save;

  if (dialog_is_active()) {
    dialog_update();
    if (!dialog_is_active() && travel_warp) {
      uint8_t event_roll = fast_rng(data->frame_counter);

      if (event_roll < 30) {
        dialog_start("WARNING: PIRATE");
        dialog_start("INTERCEPTED SIGNAL!");

        uint8_t local_lvl = station_registry[save->current_station_id].tech_level;
        generate_wild_encounter(&wild_enemy, local_lvl - 2, local_lvl + 2, data->frame_counter);

        fade_out_black();
        battle_init(data, &data->current_save->player_ship, &wild_enemy);
        fade_in_black();
        data->state = BATTLE;
      } else {
        uint8_t next_station = fast_rng(data->frame_counter) % 3;
        save->current_station_id = next_station;

        save->market_seed = fast_rng(save->market_seed);

        sram_write(0, (uint8_t*)save, sizeof(SaveData));

        dialog_start("Arrived at destination.");
        station_init(data);
      }
    }
    return;
  }

  static uint8_t target_cursor = 0;
  char buf[20];

  if (INPUT_PRESSED(PAD_UP)) {
    target_cursor = (target_cursor == 0) ? MAX_STATIONS - 1 : target_cursor - 1;
    travel_draw_info = 1;
  }
  if (INPUT_PRESSED(PAD_DOWN)) {
    target_cursor = (target_cursor == MAX_STATIONS - 1) ? 0 : target_cursor + 1;
    travel_draw_info = 1;
  }

  if (INPUT_PRESSED(PAD_B)) {
    station_init(data);
    return;
  }

  if (INPUT_PRESSED(PAD_A)) {
    if (target_cursor == save->current_station_id) {
      dialog_start("Already at destination.");
    } else {
      dialog_start("Calculating Warp...");
    }
    return;
  }

  if (travel_draw_full) {
    clear_bg();

    display_message_bg(0, 0, "\\\\\\ NAV-COMPUTER ///");
    display_message_bg(0, 1, "====================");

    display_message_bg(0, 3, "SELECT DESTINATION:");

    display_message_bg(0, 10, "--------------------");

    display_message_bg(0, 17, "[A] JUMP  [B] ABORT");

    travel_draw_full = 0;
  }

  if (travel_draw_info) {
    const StationDef* target_station = &station_registry[target_cursor];

    for(uint8_t i = 0; i < MAX_STATIONS; i++) {
      if (i > 4) break;

      if (target_cursor == i) display_message_bg(1, i + 5, ">");
      else display_message_bg(1, i + 5, " ");

      if (i == save->current_station_id) sprintf(buf, "[X] %s", station_registry[i].name);
      else sprintf(buf, "    %s", station_registry[i].name);

      display_message_bg(3, i + 5, buf);
    }

    clear_lines_bg(12, 15);

    display_message_bg(1, 12, "+-SYS-+");
    display_message_bg(1, 13, "|     |");
    display_message_bg(1, 14, "|     |");
    display_message_bg(1, 15, "+-----+");

    if (target_cursor % 2 == 0) {
      display_message_bg(2, 13, " . * ");
      display_message_bg(2, 14, "  O  ");
    } else {
      display_message_bg(2, 13, " (O) ");
      display_message_bg(2, 14, " * . ");
    }

    display_message_bg(9, 12, "TARGET DATA:");

    uint8_t dist = (target_cursor == save->current_station_id) ? 0 : (target_cursor * 12 + 4);
    sprintf(buf, "DIST: %u LY", dist);
    display_message_bg(9, 13, buf);

    sprintf(buf, "HAZ: LVL %u", target_station->threat);
    display_message_bg(9, 14, buf);

    sprintf(buf, "ZONE: SEC-%u", target_cursor);
    display_message_bg(9, 15, buf);

    travel_draw_info = 0;
  }
}
