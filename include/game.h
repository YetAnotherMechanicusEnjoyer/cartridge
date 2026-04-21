#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef enum {
  TITLE,
  MINIGAME,
  GAMEOVER
} GameState;

typedef struct {
  /* State */
  GameState state;

  /* Player */
  uint8_t player_x;
  uint8_t player_y;

  /* Score */
  uint16_t best_score;

  /* Joypad */
  uint8_t joypad_current;
  uint8_t joypad_previous;

  /* Engine */
  uint8_t frame_counter;
} GameData;

#endif // !GAME_H
