#include "encounter.h"
#include <gb/gb.h>
#include <string.h>

#define M_TACKLE { "TACKLE", 40, T_NORMAL, EFF_DAMAGE }
#define M_SCRATCH { "SCRATCH", 40, T_NORMAL, EFF_DAMAGE }
#define M_EMBER { "EMBER", 40, T_FIRE, EFF_DAMAGE }
#define M_VINEWHIP { "VINE WHIP", 45, T_GRASS, EFF_DAMAGE }
#define M_GROWL { "GROWL", 0, T_NORMAL, EFF_ATTACK_DOWN }
#define M_TAILWHIP { "TAIL WHIP" , 0, T_NORMAL, EFF_DEFENSE_DOWN }
#define M_EMPTY { "", 0, T_NORMAL, EFF_DAMAGE }

#define SPECIES_COUNT 4
const SpeciesDef species_registry[SPECIES_COUNT] = {
  {
    .name="Bulbasaur", .type1=T_GRASS, .type2=T_NORMAL,
    .base_hp=10, .base_atk=9, .base_def=9, .base_spd=9,
    .default_moves={M_TACKLE, M_GROWL, M_VINEWHIP, M_EMPTY}
  },
  {
    .name="Charmander", .type1=T_FIRE, .type2=T_NORMAL,
    .base_hp=9, .base_atk=10, .base_def=8, .base_spd=10,
    .default_moves={M_SCRATCH, M_GROWL, M_EMBER, M_EMPTY}
  },
  {
    .name="Squirtle", .type1=T_WATER, .type2=T_NORMAL,
    .base_hp=10, .base_atk=9, .base_def=11, .base_spd=8,
    .default_moves={M_TACKLE, M_TAILWHIP, M_EMPTY, M_EMPTY}
  },
  {
    .name="Rattata", .type1=T_NORMAL, .type2=T_NORMAL,
    .base_hp=8, .base_atk=11, .base_def=7, .base_spd=12,
    .default_moves={M_TACKLE, M_TAILWHIP, M_EMPTY, M_EMPTY}
  },
};

static uint8_t get_random(uint8_t frame_counter) {
  static uint8_t seed = 42;
  seed = (seed * 17) + DIV_REG + frame_counter;
  return seed;
}

void generate_wild_encounter(BattleEntity* target, uint8_t min_lvl, uint8_t max_lvl, uint8_t frame_counter) {
  uint8_t rng = get_random(frame_counter);
  uint8_t species_idx = rng % SPECIES_COUNT;
  const SpeciesDef* base = &species_registry[species_idx];

  rng = get_random(frame_counter);
  uint8_t level = min_lvl + (rng % ((max_lvl - min_lvl) + 1));

  strcpy(target->name, base->name);
  target->level = level;
  target->type1 = base->type1;
  target->type2 = base->type2;
  target->xp = 0;

  target->max_hp = base->base_hp + (base->base_hp * level / 10) + 5;
  target->hp = target->max_hp;
  target->attack = base->base_atk + (base->base_atk * level / 10);
  target->defense = base->base_def + (base->base_def * level / 10);
  target->speed = base->base_spd + (base->base_spd * level / 10);

  for (uint8_t i = 0; i < 4; i++) {
    target->moves[i] = base->default_moves[i];
  }
}
