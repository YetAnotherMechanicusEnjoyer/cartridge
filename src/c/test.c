#include "asm.h"
#include "asm_wrapper.h"
#include "game.h"
#include "input.h"
#include "timers.h"

#define TIMER_INVICIBLE 0

void test_init(GameData* data) {
  data->player_x = 80;
  data->player_y = 72;
  data->best_score = 0;
  display_message(0, 0, "SCORE:");
  update_score_display(6, 0, data->best_score);
}

void test_game(GameData* data) {
  if (INPUT_HELD(PAD_UP) && data->player_y > 16) data->player_y -= 2;
  if (INPUT_HELD(PAD_DOWN) && data->player_y < 152) data->player_y += 2;
  if (INPUT_HELD(PAD_LEFT) && data->player_x > 8) data->player_x -= 2;
  if (INPUT_HELD(PAD_RIGHT) && data->player_x < 160) data->player_x += 2;


  if (check_timer(TIMER_INVICIBLE)) {
    clear_message(17, 0, 3);
    clear_line(1);
  }

  if (INPUT_PRESSED(PAD_A)) {
    if (timer_counters[TIMER_INVICIBLE] == 0) {
      set_timer(TIMER_INVICIBLE, 60);
      uint16_t new_score = data->best_score + 10;
      if (new_score < data->best_score) {
        data->state = GAMEOVER;
        return;
      }
      data->best_score = new_score;
      update_score_display(6, 0, data->best_score);
      display_message(17, 0, "...");
    } else {
      display_message(0, 1, "/!\\ Wait /!\\");
    }
  }

  move_sprite(0, data->player_x, data->player_y);

  if (INPUT_RELEASED(PAD_SELECT)) {
    clear_message(0, 1, 11);
    data->state = GAMEOVER;
  }
}
