#ifndef DODGE_TYPES_H
#define DODGE_TYPES_H

#include <stdint.h>
#include "dodge_config.h"

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t speed;
    uint8_t active;
    uint8_t enabled;
    uint8_t delay;

    uint8_t zigzag;
    uint8_t direction;
} Meteor;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t active;
} StarCollectible;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t active;
} ShieldPowerup;

typedef struct {
    uint8_t state;
    uint8_t player_x;

    Meteor meteors[METEOR_COUNT];
    ShieldPowerup shield_powerup;
    StarCollectible star;

    uint8_t shield_active;
    uint8_t invincible_timer;
    uint16_t shield_spawn_timer;
    uint16_t star_spawn_timer;

    uint8_t score_frame;
    uint8_t timer_frame;

    uint8_t level;
    uint16_t level_timer;

    uint8_t transition_timer;
    uint8_t transition_redraw_timer;

    uint8_t meteor_anim_timer;
    uint8_t meteor_anim_frame;
    uint8_t player_anim_timer;

    uint8_t seed;
} DodgeState;


























#endif
