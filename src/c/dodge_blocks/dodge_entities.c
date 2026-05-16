#include "dodge_entities.h"
#include "dodge_config.h"
#include "dodge_utils.h"
#include "dodge_ui.h"
#include "dodge_audio.h"
#include "input.h"

#include <gb/gb.h>
#include <stdint.h>

static uint8_t meteor_count_for_level(DodgeState* dodge)
{
    if (dodge->level <= 2) {
        return 3;
    }

    if (dodge->level <= 4) {
        return 4;
    }

    return 5;
}

static uint8_t speed_for_level(DodgeState* dodge)
{
    if (dodge->level <= 2) {
        return 1;
    }

    if (dodge->level <= 4) {
        return 2;
    }

    if (dodge->level <= 7) {
        return 3;
    }

    return 4;
}

static uint8_t spawn_gap_for_level(DodgeState* dodge)
{
    if (dodge->level <= 2) {
        return 42;
    }

    if (dodge->level <= 4) {
        return 32;
    }

    if (dodge->level <= 7) {
        return 24;
    }

    return 16;
}


static void setup_meteor_type(DodgeState* dodge, uint8_t i)
{
    if (dodge->level >= ZIGZAG_START_LEVEL && i == ZIGZAG_METEOR_INDEX) {
        dodge->meteors[i].zigzag = 1;
        dodge->meteors[i].direction = dodge_rand8(dodge) & 1;
    } else {
        dodge->meteors[i].zigzag = 0;
        dodge->meteors[i].direction = 0;
    }
}

static void hide_player(void)
{
    move_sprite(PLAYER_SPRITE, 0, 0);
}

void dodge_entities_show_player(DodgeState* dodge)
{
    move_sprite(PLAYER_SPRITE, dodge->player_x, PLAYER_Y);
}

static void update_player_sprite(DodgeState* dodge, uint8_t moving)
{
    uint8_t tile;

    if (moving) {
        dodge->player_anim_timer++;

        if (dodge->player_anim_timer & 8) {
            tile = TILE_PLAYER_BOOST;
        } else {
            tile = TILE_PLAYER;
        }
    } else {
        dodge->player_anim_timer = 0;
        tile = TILE_PLAYER;
    }

    set_sprite_tile(PLAYER_SPRITE, tile);

    if (dodge->invincible_timer > 0 && (dodge->invincible_timer & 8)) {
        hide_player();
    } else {
        dodge_entities_show_player(dodge);
    }
}

static void update_meteor_zigzag(DodgeState* dodge, uint8_t i)
    {
        if (!dodge->meteors[i].zigzag) {
            return;
        }

        if (dodge->meteors[i].direction == 0) {
            if (dodge->meteors[i].x > PLAYER_MIN_X) {
                dodge->meteors[i].x--;
            } else {
                dodge->meteors[i].direction = 1;
            }
        } else {
            if (dodge->meteors[i].x < PLAYER_MAX_X) {
                dodge->meteors[i].x++;
            } else {
                dodge->meteors[i].direction = 0;
            }
        }
    }


static void hide_meteor(DodgeState* dodge, uint8_t i)
{
    dodge->meteors[i].active = 0;
    move_sprite(FIRST_METEOR_SPRITE + i, 0, 0);
}

static void hide_meteors(DodgeState* dodge)
{
    uint8_t i;

    for (i = 0; i < METEOR_COUNT; i++) {
        hide_meteor(dodge, i);
    }
}

void dodge_entities_hide_shield_powerup(DodgeState* dodge)
{
    dodge->shield_powerup.active = 0;
    move_sprite(SHIELD_SPRITE, 0, 0);
}

void dodge_entities_hide_all(DodgeState* dodge)
{
    hide_player();
    hide_meteors(dodge);
    dodge_entities_hide_shield_powerup(dodge);
}

void dodge_entities_hide_visual(DodgeState* dodge)
{
    uint8_t i;

    move_sprite(PLAYER_SPRITE, 0, 0);

    for (i = 0; i < METEOR_COUNT; i++) {
        move_sprite(FIRST_METEOR_SPRITE + i, 0, 0);
    }

    move_sprite(SHIELD_SPRITE, 0, 0);
}

void dodge_entities_show_active(DodgeState* dodge)
{
    uint8_t i;

    dodge_entities_show_player(dodge);

    for (i = 0; i < METEOR_COUNT; i++) {
        if (dodge->meteors[i].active) {
            move_sprite(FIRST_METEOR_SPRITE + i, dodge->meteors[i].x, dodge->meteors[i].y);
        }
    }

    if (dodge->shield_powerup.active) {
        move_sprite(SHIELD_SPRITE, dodge->shield_powerup.x, dodge->shield_powerup.y);
    }
}

static void prepare_meteor_spawn(DodgeState* dodge, uint8_t i, uint8_t delay)
{
    dodge->meteors[i].enabled = 1;
    dodge->meteors[i].active = 0;
    dodge->meteors[i].delay = delay;
    dodge->meteors[i].x = dodge_random_lane_x(dodge);
    dodge->meteors[i].y = 16;
    dodge->meteors[i].speed = speed_for_level(dodge);
    setup_meteor_type(dodge, i);
    move_sprite(FIRST_METEOR_SPRITE + i, 0, 0);
}

static void spawn_meteor_from_top(DodgeState* dodge, uint8_t i)
{
    dodge->meteors[i].enabled = 1;
    dodge->meteors[i].x = dodge_random_lane_x(dodge);
    dodge->meteors[i].y = 16;
    dodge->meteors[i].speed = speed_for_level(dodge);
    dodge->meteors[i].active = 1;
    dodge->meteors[i].delay = 0;
    setup_meteor_type(dodge, i);
    move_sprite(FIRST_METEOR_SPRITE + i, dodge->meteors[i].x, dodge->meteors[i].y);
}

void dodge_entities_reset_meteors_for_level(DodgeState* dodge)
{
    uint8_t i;
    uint8_t count;
    uint8_t gap;

    count = meteor_count_for_level(dodge);
    gap = spawn_gap_for_level(dodge);

    for (i = 0; i < METEOR_COUNT; i++) {
        if (i < count) {
            prepare_meteor_spawn(dodge, i, i * gap);
        } else {
            dodge->meteors[i].enabled = 0;
            dodge->meteors[i].active = 0;
            dodge->meteors[i].delay = 0;
            dodge->meteors[i].zigzag = 0;
            dodge->meteors[i].direction = 0;
            move_sprite(FIRST_METEOR_SPRITE + i, 0, 0);
        }
    }

    spawn_meteor_from_top(dodge, 0);
}

static void spawn_shield_powerup(DodgeState* dodge)
{
    dodge->shield_powerup.x = dodge_random_lane_x(dodge);
    dodge->shield_powerup.y = 16;
    dodge->shield_powerup.active = 1;

    move_sprite(SHIELD_SPRITE, dodge->shield_powerup.x, dodge->shield_powerup.y);
}

void dodge_entities_init(DodgeState* dodge)
{
    uint8_t i;

    set_sprite_tile(PLAYER_SPRITE, TILE_PLAYER);
    move_sprite(PLAYER_SPRITE, 0, 0);

    for (i = 0; i < METEOR_COUNT; i++) {
        set_sprite_tile(FIRST_METEOR_SPRITE + i, TILE_METEORITE);
        move_sprite(FIRST_METEOR_SPRITE + i, 0, 0);

        dodge->meteors[i].enabled = 0;
        dodge->meteors[i].active = 0;
        dodge->meteors[i].delay = 0;
    }

    set_sprite_tile(SHIELD_SPRITE, TILE_SHIELD);
    move_sprite(SHIELD_SPRITE, 0, 0);

    dodge->player_x = 80;
}

void dodge_entities_update_player(DodgeState* dodge)
{
    uint8_t moving;

    moving = 0;

    if (INPUT_HELD(PAD_LEFT) && dodge->player_x > PLAYER_MIN_X) {
        dodge->player_x -= PLAYER_SPEED;
        moving = 1;
    }

    if (INPUT_HELD(PAD_RIGHT) && dodge->player_x < PLAYER_MAX_X) {
        dodge->player_x += PLAYER_SPEED;
        moving = 1;
    }

    if (dodge->invincible_timer > 0) {
        dodge->invincible_timer--;
    }

    update_player_sprite(dodge, moving);
}

static void update_meteor_spawn_delay(DodgeState* dodge, uint8_t i)
{
    if (dodge->meteors[i].delay > 0) {
        dodge->meteors[i].delay--;
    }

    if (dodge->meteors[i].delay == 0) {
        spawn_meteor_from_top(dodge, i);
    }
}

void dodge_entities_update_meteor_animation(DodgeState* dodge)
{
    uint8_t i;
    uint8_t tile;

    dodge->meteor_anim_timer++;

    if (dodge->meteor_anim_timer < 8) {
        return;
    }

    dodge->meteor_anim_timer = 0;
    dodge->meteor_anim_frame = !dodge->meteor_anim_frame;

    if (dodge->meteor_anim_frame) {
        tile = TILE_METEORITE2;
    } else {
        tile = TILE_METEORITE;
    }

    for (i = 0; i < METEOR_COUNT; i++) {
        set_sprite_tile(FIRST_METEOR_SPRITE + i, tile);
    }
}

uint8_t dodge_entities_update_meteors(DodgeState* dodge)
{
    uint8_t i;

    for (i = 0; i < METEOR_COUNT; i++) {
        if (!dodge->meteors[i].enabled) {
            continue;
        }

        if (!dodge->meteors[i].active) {
            update_meteor_spawn_delay(dodge, i);
            continue;
        }

        dodge->meteors[i].y += dodge->meteors[i].speed;
        update_meteor_zigzag(dodge, i);

        if (dodge->meteors[i].y > 160) {
            hide_meteor(dodge, i);
            prepare_meteor_spawn(dodge, i, spawn_gap_for_level(dodge) + (dodge_rand8(dodge) & 31));
            continue;
        }

        move_sprite(FIRST_METEOR_SPRITE + i, dodge->meteors[i].x, dodge->meteors[i].y);

        if (dodge->invincible_timer == 0 &&
            dodge_collide8(dodge->player_x, PLAYER_Y, dodge->meteors[i].x, dodge->meteors[i].y)) {
            if (dodge->shield_active) {
                dodge->shield_active = 0;
                dodge->invincible_timer = 60;

                dodge_ui_draw_shield_status(dodge);
                dodge_audio_shield();

                hide_meteor(dodge, i);
                prepare_meteor_spawn(dodge, i, 30);
            } else {
                return 1;
            }
        }
    }

    return 0;
}

void dodge_entities_update_shield_powerup(DodgeState* dodge)
{
    if (!dodge->shield_powerup.active && !dodge->shield_active) {
        dodge->shield_spawn_timer++;

        if (dodge->shield_spawn_timer >= SHIELD_SPAWN_FRAMES) {
            dodge->shield_spawn_timer = 0;
            spawn_shield_powerup(dodge);
        }

        return;
    }

    if (!dodge->shield_powerup.active) {
        return;
    }

    dodge->shield_powerup.y += 1;

    if (dodge->shield_powerup.y > 160) {
        dodge_entities_hide_shield_powerup(dodge);
        return;
    }

    move_sprite(SHIELD_SPRITE, dodge->shield_powerup.x, dodge->shield_powerup.y);

    if (dodge_collide8(dodge->player_x, PLAYER_Y, dodge->shield_powerup.x, dodge->shield_powerup.y)) {
        dodge_entities_hide_shield_powerup(dodge);

        dodge->shield_active = 1;

        dodge_ui_draw_shield_status(dodge);
        dodge_audio_shield();
    }

    }
