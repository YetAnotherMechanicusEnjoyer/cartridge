#include "core.h"
#include <stdio.h>

int main(void) {
  uint8_t result = add_asm(10, 5);

  printf("\n10 + 5 = %d\n", result);
  printf("\nPress Start\n");

  while(1) {
    wait_vbl_done();

    uint8_t joypad_state = joypad();

    if (joypad_state & J_START) {
      printf("Game Started!\n");
      delay(500);
    }
  }
}
