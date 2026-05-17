#ifndef ENCOUNTER_H
#define ENCOUNTER_H

#include "battle.h"
#include <stdint.h>

typedef struct {
  char name[12];
  Type type1;
  Type type2;
  uint8_t base_hp;
  uint8_t base_atk;
  uint8_t base_def;
  uint8_t base_spd;
  Move default_moves[4];
} SpeciesDef;

void generate_wild_encounter(BattleEntity* target, uint8_t min_lvl, uint8_t max_lvl, uint8_t frame_counter);

#endif /* ENCOUNTER_H */
