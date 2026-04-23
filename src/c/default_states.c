#include "asm_wrapper.h"
#include "game.h"
#include <gb/gb.h>
#include <string.h>

uint8_t scroll_x_val = 0;

void title_state(GameData* data, uint8_t keys_pressed) {
  move_sprite(0, 0, 0);
  display_middle(2, "CHOOSE A GAME &");
  display_middle(3, "PRESS START");
  display_middle(4, "TO PLAY");

  for(uint8_t i = 0; i < data->n_games; i++) {
    char buff[21] = {0};
    clear_line(i + 6);
    if (data->current_game_id == i) {
      strcpy(buff, ">");
    }
    strcat(buff, data->games[i].name);
    display_message(2, i + 6, buff);
  }

  if (keys_pressed & J_START) {
    clear_window();
    data->state = GAME;
    scroll_x_val = 0;
    data->games[data->current_game_id].init(data);
  }
  if (keys_pressed & J_UP) data->current_game_id = data->current_game_id > 0 ? data->current_game_id - 1 : data->n_games - 1;
  if (keys_pressed & J_DOWN) data->current_game_id = data->current_game_id < data->n_games - 1 ? data->current_game_id + 1 : 0;
}

void gameover_state(GameData* data, uint8_t keys_pressed) {
  move_sprite(0, 0, 0);
  display_middle(8, "GAME OVER");
  display_middle(10, "PRESS START");

  if (keys_pressed & J_START) {
    clear_window();
    data->state = TITLE;
  }
}
