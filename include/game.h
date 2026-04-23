#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef enum {
  TITLE,
  GAME,
  GAMEOVER
} GameState;

typedef struct GameData GameData;

typedef void (*GameFunction)(GameData*, uint8_t);

typedef void (*InitFunction)(GameData*);

typedef struct {
  char* name;
  GameFunction game;
  InitFunction init;
} Game;

struct GameData {
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

  /* Games */
  uint8_t current_game_id;
  uint8_t n_games;
  Game* games;
};

void title_state(GameData* data, uint8_t keys_pressed);
void gameover_state(GameData* data, uint8_t keys_pressed);

void test_init(GameData* data);
void test_game(GameData* data, uint8_t keys_pressed);

#endif // !GAME_H
