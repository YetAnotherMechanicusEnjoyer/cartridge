#ifndef BATTLE_H
#define BATTLE_H

#include <stdint.h>

typedef enum {
  T_NORMAL,
  T_FIRE,
  T_WATER,
  T_GRASS,
} Type;

typedef enum {
  EFF_DAMAGE,
  EFF_DEFENSE_DOWN,
  EFF_DEFENSE_UP,
  EFF_ATTACK_DOWN,
  EFF_ATTACK_UP,
} MoveEffect;

typedef struct {
  char name[12];
  uint8_t power;
  Type type;
  MoveEffect effect;
} Move;

typedef struct BattleEntity {
  char name[12];
  uint8_t level;
  uint16_t hp;
  uint16_t max_hp;
  uint8_t attack;
  uint8_t defense;
  uint8_t speed;
  uint16_t xp;
  Type type1;
  Type type2;
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

#endif /* BATTLE_H */
