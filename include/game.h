#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include "trader.h"

typedef enum {
  TITLE,
  GAME,
  STATION,
  MARKET,
  TRAVEL,
  BATTLE,
  GAMEOVER
} GameState;

typedef struct GameData GameData;

typedef void (*GameFunction)(GameData*);

typedef void (*InitFunction)(GameData*);

typedef struct {
  char* name;
  uint8_t bank;
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
  uint16_t score;

  /* Engine */
  uint8_t frame_counter;

  /* Games */
  uint8_t current_game_id;
  uint8_t n_games;

  /* Save */
  SaveData* current_save;
  const Game* games;
};

void title_state(GameData* data);

void gameover_init(GameData* data);
void gameover_state(GameData* data);

void test_init(GameData* data);
void test_game(GameData* data);

void station_init(GameData* data);
void station_state(GameData* data);

void travel_init(GameData* data);
void travel_state(GameData* data);

#endif // !GAME_H
