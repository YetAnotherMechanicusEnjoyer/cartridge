#include "asm.h"
#include "asm_wrapper.h"
#include "game.h"
#include "input.h"
#include "sram.h"
#include "timers.h"

#define TIMER_INVICIBLE 0

uint8_t col_x = 0;
uint8_t col_y = 0;

void test_init(GameData* data) {
  data->player_x = 25;
  data->player_y = 20;
  data->score = 0;
  move_win(7, 144);
  display_message(0, 0, "SCORE:");
  update_score_display(6, 0, data->current_save.high_score);
}

void test_game(GameData* data) {
  uint8_t next_x = data->player_x;
  uint8_t next_y = data->player_y;

  if (INPUT_HELD(PAD_UP) && data->player_y > 16) next_y -= 1;
  if (INPUT_HELD(PAD_DOWN) && data->player_y < 152) next_y += 1;
  if (INPUT_HELD(PAD_LEFT) && data->player_x > 8) next_x -= 1;
  if (INPUT_HELD(PAD_RIGHT) && data->player_x < 160) next_x += 1;

  col_x = next_x;
  col_y = next_y;

  if (check_collision() == 0) {
    data->player_x = next_x;
    data->player_y = next_y;
  }

  if (check_timer(TIMER_INVICIBLE)) {
    move_win(7, 144);
    clear_message(17, 0, 3);
    clear_line(1);
  }

  if (INPUT_PRESSED(PAD_A)) {
    if (timer_counters[TIMER_INVICIBLE] == 0) {
      set_timer(TIMER_INVICIBLE, 60);
      uint16_t new_score = data->score + 10;
      if (new_score < data->score) {
        data->state = GAMEOVER;
        return;
      }
      data->score = new_score;
      move_win(7, 112);
      update_score_display(6, 0, data->score);
      display_message(17, 0, "...");
    } else {
      display_message(0, 1, "/!\\ Wait /!\\");
    }
  }

  move_sprite(0, data->player_x, data->player_y);

  if (INPUT_RELEASED(PAD_SELECT)) {
    clear_window();
    if (data->current_save.high_score < data->score) {
      data->current_save.high_score = data->score;
      sram_write(0, (uint8_t*)&data->current_save, sizeof(SaveData));
    }
    move_win(7, 0);
    data->state = GAMEOVER;
  }
}
