#include "asm.h"
#include "battle_func.h"
#include "encounter.h"
#include "game.h"
#include "dialog.h"
#include "sram.h"
#include "trader.h"
#include <string.h>

BattleEntity wild_enemy;

void travel_state(GameData* data) {
  SaveData* save = data->current_save;

  if (dialog_is_active()) {
    dialog_update();
    return;
  }

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
    data->state = STATION;
  }
}
