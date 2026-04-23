#include "asm.h"
#include "asm_wrapper.h"
#include "game.h"

void test_init(GameData* data) {
    data->player_x = 80;
    data->player_y = 72;
    display_message(0, 0, "SCORE:");
    update_score_display(6, 0, data->best_score);
}

void test_game(GameData* data, uint8_t keys_pressed) {
  if (data->joypad_previous & J_UP && data->player_y > 16) data->player_y -= 2;
  if (data->joypad_current & J_DOWN && data->player_y < 152) data->player_y += 2;
  if (data->joypad_current & J_LEFT && data->player_x > 8) data->player_x -= 2;
  if (data->joypad_current & J_RIGHT && data->player_x < 160) data->player_x += 2;

  if (keys_pressed & J_A) {
    data->best_score++;
    save_score(data->best_score);
    update_score_display(6, 0, data->best_score);
  }

  move_sprite(0, data->player_x, data->player_y);

  if (keys_pressed & J_SELECT) {
    clear_message(0, 1, 11);
    data->state = GAMEOVER;
  }
}
