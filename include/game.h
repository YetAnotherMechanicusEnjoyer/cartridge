#ifndef GAME_H
#define GAME_H

#include "battle.h"
#include <stdint.h>

typedef enum {
  TITLE,
  GAME,
  BATTLE,
  GAMEOVER
} GameState;

typedef struct GameData GameData;

typedef void (*GameFunction)(GameData*);

typedef void (*InitFunction)(GameData*);

typedef struct {
  char* name;
  GameFunction game;
  InitFunction init;
} Game;

typedef struct {
  uint8_t save_initialized;
  uint16_t high_score;
  BattleEntity player_bentity;
} SaveData;


struct GameData {
  /* State */
  GameState state;

  /* Player */
  uint8_t player_x;
  uint8_t player_y;

  /* Score */
  uint16_t score;

  /* Engine */
  uint8_t frame_counter;

  /* Games */
  uint8_t current_game_id;
  uint8_t n_games;

  /* Save */
  SaveData current_save;
  const Game* games;
};

void title_state(GameData* data);

void gameover_init(GameData* data);
void gameover_state(GameData* data);

void test_init(GameData* data);
void test_game(GameData* data);

#endif // !GAME_H
