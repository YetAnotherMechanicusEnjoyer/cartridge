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

void gameover_init(GameData* data) {
  (void)data;
  clear_window();
  move_win(7, 0);
}

void gameover_state(GameData* data) {
  move_sprite(0, 0, 0);
  display_middle(8, "GAME OVER");
  display_middle(10, "PRESS START");

  if (INPUT_PRESSED(PAD_START)) {
    clear_window();
    data->state = TITLE;
  }
}
