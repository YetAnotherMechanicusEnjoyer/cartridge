#include "asm_wrapper.h"
#include "dialog.h"
#include <gb/gb.h>
#include "input.h"

DialogManager dm;

void clear_dialog_area() {
  for (uint8_t i = 1; i < 3; i++) {
    clear_message(1, i, 18);
  }
}

void dialog_start(const char* text) {
  dm.current_text = text;
  dm.char_ptr = 0;
  dm.x = 1;
  dm.y = 1;
  dm.timer = 0;
  dm.state = D_TYPING;

  display_message(0, 0, "/==================\\");
  display_message(0, 3, "\\==================/");
  move_win(7, 112);
  clear_dialog_area();
}

void dialog_update() {
  if (dm.state == D_IDLE) return;

  if (dm.state == D_WAITING_INPUT && INPUT_PRESSED(PAD_A)) {
    clear_dialog_area();
    dm.x = 1;
    dm.y = 1;
    dm.state = D_TYPING;
    return;
  }

  if (dm.state == D_TYPING) {
    dm.timer++;

    uint8_t current_speed = INPUT_HELD(PAD_A) ? 1 : TYPE_SPEED;

    if (dm.timer >= current_speed) {
      dm.timer = 0;
      char c = dm.current_text[dm.char_ptr];

      if (c == '\0') {
        dm.state = D_DONE;
        return;
      }

      if (c == ' ') {
        uint16_t temp_ptr = dm.char_ptr + 1;
        uint8_t word_len = 0;
        while (dm.current_text[temp_ptr] != ' ' && dm.current_text[temp_ptr] != '\0') {
          word_len++;
          temp_ptr++;
        }
        if (dm.x + word_len > DIALOG_MAX_WIDTH) {
          dm.x = 1;
          dm.y++;
          dm.char_ptr++;
          c = dm.current_text[dm.char_ptr];
        }
      }

      if (dm.y > DIALOG_MAX_LINES) {
        dm.state = D_WAITING_INPUT;
        return;
      }

      char str[2] = {c, '\0'};
      display_message(dm.x, dm.y, str);

      dm.x++;
      dm.char_ptr++;
    }
  }

  if (dm.state == D_DONE && INPUT_PRESSED(PAD_A)) {
    move_win(7, 144);
    dm.state = D_IDLE;
  }
}

uint8_t dialog_is_active() {
  return dm.state != D_IDLE;
}
