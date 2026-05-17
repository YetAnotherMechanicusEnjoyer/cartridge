#include "audio.h"
#include "asm.h"
#include "asm_wrapper.h"
#include "dialog.h"
#include "game.h"
#include "banking.h"
#include "input.h"
#include <gb/gb.h>
#include <string.h>

uint8_t scroll_x_val = 0;

void title_state(GameData* data) {
  move_sprite(0, 0, 0);
  static uint8_t draw = 1;
  if (INPUT_PRESSED(PAD_UP)) {
    data->current_game_id = data->current_game_id > 0 ? data->current_game_id - 1 : data->n_games - 1;
    draw = 1;
  }
  if (INPUT_PRESSED(PAD_DOWN)) {
    data->current_game_id = data->current_game_id < data->n_games - 1 ? data->current_game_id + 1 : 0;
    draw = 1;
  }

  if (draw) {
    display_middle(2, "CHOOSE A GAME &");
    display_middle(3, "PRESS START");

    for(uint8_t i = 0; i < data->n_games; i++) {
      char buff[21] = {0};
      if (data->current_game_id == i) strcpy(buff, ">");
      else strcpy(buff, " ");
      strcat(buff, data->games[i].name);
      clear_line(i + 6);
      display_message(2, i + 6, buff);
    }
    draw = 0;
  }

  if (INPUT_PRESSED(PAD_A)) {
    sfx_confirm();
    clear_window();
    draw = 1;
    data->state = GAME;
    scroll_x_val = 0;
    if (data->games[data->current_game_id].bank != 0)
      rom_switch(data->games[data->current_game_id].bank);
    data->games[data->current_game_id].init(data);
  }
}

const char* game_over_texts[3] = {
    "YOUR SHIP IS NOW AN ASHTRAY.",
    "PIRATES THANK YOU FOR THE FREE SCRAP.",
    "YOU DIED ALONE IN THE COLD VACUUM."
};

void gameover_init(GameData* data) {
  clear_window();
  clear_bg();
  move_win(7, 144);
  data->state = GAMEOVER;

  display_message_bg(2, 1,   ".      * .");
  display_message_bg(16, 2,  ".");
  display_message_bg(1, 7,   "    .");
  display_message_bg(18, 8,  "*");

  display_message_bg(5, 4,   "___/|\\ ");
  display_message_bg(13, 4,  " ___");
  display_message_bg(4, 5,  "<  X R K ");
  display_message_bg(13, 5, " >");
  display_message_bg(5, 6,   "~~~\\|/ ");
  display_message_bg(13, 6,  " ~~~");

  display_message_bg(3, 3,   "x");
  display_message_bg(15, 3,  "o");
  display_message_bg(12, 7,  "#");
  display_message_bg(7, 9,   "x");

  SaveData* save = data->current_save;

  save->market_seed = (save->market_seed * 17) + fast_rng(save->market_seed + data->frame_counter) + data->frame_counter;
  uint8_t game_over_index = save->market_seed % 3;
  dialog_start(">>> CRITICAL FAILURE <<<");
  dialog_start(game_over_texts[game_over_index]);
  dialog_start("/!\\ GAME OVER /!\\");
}

void gameover_state(GameData* data) {
  move_sprite(0, 0, 0);

  if (data->frame_counter % 30 == 0) {
    scroll_bkg(1, 0);
  } else if (data->frame_counter % 31 == 0) {
    scroll_bkg(-1, 0);
  } else {
    move_bkg(0, 0);
  }

  if (dialog_is_active()) {
    dialog_update();
    return;
  }

  clear_window();
  clear_bg();
  data->state = TITLE;
}
