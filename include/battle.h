#ifndef BATTLE_H
#define BATTLE_H

#include "game.h"
#include <stdint.h>

typedef struct {
  char name[12];
  uint8_t power;
} Move;

typedef struct {
  char name[12];
  uint8_t level;
  uint16_t hp;
  uint16_t max_hp;
  uint8_t attack;
  uint8_t defense;
  uint8_t speed;
  Move moves[4];
} BattleEntity;

typedef enum {
  ID_PLAYER = 0,
  ID_OPPONENT = 1,
} FighterID;

typedef enum {
  B_INIT,
  B_WAIT_INTRO,
  B_MAIN_MENU,
  B_MOVE_MENU,
  B_SPEED_CHECK,
  B_EXECUTE_TURN,
  B_WAIT_ANIM,
  B_CHECK_DEATH,
  B_END_BATTLE,
} BattleState;

typedef struct {
  BattleEntity* player;
  BattleEntity* opponent;
  BattleState state;

  uint8_t menu_cursor;

  FighterID first_attacker;
  FighterID current_attacker;

  uint8_t turn_step;
  uint8_t player_move_idx;
  uint8_t enemy_move_idx;
  uint8_t is_player_faster;
} BattleManager;

void battle_init(GameData* data, BattleEntity* p, BattleEntity* o);
void battle_update(GameData* data);

#endif /* BATTLE_H */
