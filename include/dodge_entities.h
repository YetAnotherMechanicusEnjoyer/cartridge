#ifndef DODGE_ENTITIES_H
#define DODGE_ENTITIES_H

#include <stdint.h>
#include "dodge_type.h"

void dodge_entities_init(DodgeState* dodge);

void dodge_entities_hide_all(DodgeState* dodge);
void dodge_entities_hide_visual(DodgeState* dodge);
void dodge_entities_show_active(DodgeState* dodge);

void dodge_entities_show_player(DodgeState* dodge);
void dodge_entities_hide_shield_powerup(DodgeState* dodge);

void dodge_entities_reset_meteors_for_level(DodgeState* dodge);

void dodge_entities_update_player(DodgeState* dodge);
void dodge_entities_update_meteor_animation(DodgeState* dodge);
uint8_t dodge_entities_update_meteors(DodgeState* dodge);
void dodge_entities_update_shield_powerup(DodgeState* dodge);

void dodge_entities_update_shield_powerup(DodgeState* dodge);
uint8_t dodge_entities_update_star(DodgeState* dodge);

#endif
