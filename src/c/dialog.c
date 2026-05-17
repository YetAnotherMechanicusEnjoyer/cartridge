#include "asm_wrapper.h"
#include "dialog.h"
#include <gb/gb.h>
#include "audio.h"
#include "input.h"

DialogManager dm;

static uint8_t saved_wx = 7;
static uint8_t saved_wy = 144;

void setup_next_message(void) {
  dm.current_text = dm.queue[dm.q_read];
  dm.char_ptr = 0;
  dm.x = 1;
  dm.y = 1;
  dm.state = D_TYPING;
  display_message(0, 0, "/==================\\");
  clear_line(1);
  clear_line(2);
  display_message(0, 3, "\\==================/");
  move_win(7, 112);
}

void dialog_start(const char* text) {
  if (((dm.q_write + 1) % DIALOG_QUEUE_SIZE) == dm.q_read) return;

  dm.queue[dm.q_write] = text;

  if (dm.state == D_IDLE) {
    saved_wx = WX_REG;
    saved_wy = WY_REG;
    setup_next_message();
  }

  dm.q_write = (dm.q_write + 1) % DIALOG_QUEUE_SIZE;
}

void dialog_update(void) {
  if (dm.state == D_IDLE) return;

  if (dm.state == D_WAITING_INPUT && INPUT_PRESSED(PAD_A)) {
    clear_line(1);
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
      } else {
        play_terminal_click(sys_time);
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
    dm.q_read = (dm.q_read + 1) % DIALOG_QUEUE_SIZE;

    if (dm.q_read != dm.q_write) {
      setup_next_message();
    } else {
      clear_line(0);
      clear_line(1);
      clear_line(2);
      clear_line(3);
      move_win(saved_wx, saved_wy);
      dm.state = D_IDLE;
    }
  }
}

uint8_t dialog_is_active(void) {
  return dm.state != D_IDLE;
}
