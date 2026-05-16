#include "dodge_blocks.h"
#include "game.h"
#include "input.h"

#include "dodge_config.h"
#include "dodge_type.h"
#include "dodge_entities.h"
#include "dodge_ui.h"
#include "dodge_audio.h"
#include "dodge_save.h"

#include "../autogen/8x8/spaceship.h"
#include "../autogen/8x8/spaceship_boost.h"
#include "../autogen/8x8/meteorite.h"
#include "../autogen/8x8/meteorite2.h"

#include <gb/gb.h>
#include <stdint.h>

static DodgeState dodge;

static const unsigned char blank_tile[] = {
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00
};

static const unsigned char shield_tile[] = {
    0x3C,0x3C,
    0x42,0x42,
    0x99,0x99,
    0xA5,0xA5,
    0xA5,0xA5,
    0x99,0x99,
    0x42,0x42,
    0x3C,0x3C
};

static void clear_screen_layers(void)
{
    HIDE_WIN;
    HIDE_BKG;
    fill_bkg_rect(0, 0, 32, 32, TILE_BLANK);
    fill_win_rect(0, 0, 32, 32, TILE_BLANK);
    SHOW_BKG;
}

static void load_sprite_tiles(void)
{
    set_sprite_data(TILE_BLANK, 1, blank_tile);
    set_sprite_data(TILE_PLAYER, spaceship_TILE_COUNT, spaceship_tiles);
    set_sprite_data(TILE_PLAYER_BOOST, spaceship_boost_TILE_COUNT, spaceship_boost_tiles);
    set_sprite_data(TILE_METEORITE, meteorite_TILE_COUNT, meteorite_tiles);
    set_sprite_data(TILE_METEORITE2, meteorite2_TILE_COUNT, meteorite2_tiles);
    set_sprite_data(TILE_SHIELD, 1, shield_tile);
}

static void reset_runtime_values(GameData* data)
{
    data->score = 0;
    data->frame_counter = 0;

    dodge.player_x = 80;

    dodge.score_frame = 0;
    dodge.timer_frame = 0;

    dodge.level = 1;
    dodge.level_timer = 0;

    dodge.transition_timer = 0;
    dodge.transition_redraw_timer = 0;

    dodge.shield_spawn_timer = 0;
    dodge.shield_active = 0;
    dodge.invincible_timer = 0;

    dodge.meteor_anim_timer = 0;
    dodge.meteor_anim_frame = 0;
    dodge.player_anim_timer = 0;

    dodge.seed = DIV_REG;
}

static void start_level(GameData* data)
{
    dodge.level_timer = 0;
    dodge.score_frame = 0;
    dodge.timer_frame = 0;

    dodge.shield_spawn_timer = 0;
    dodge.shield_active = 0;
    dodge.invincible_timer = 0;

    dodge.meteor_anim_timer = 0;
    dodge.meteor_anim_frame = 0;
    dodge.player_anim_timer = 0;

    dodge_entities_hide_shield_powerup(&dodge);

    dodge_ui_draw_clean_background(&dodge, data);

    dodge_entities_show_player(&dodge);
    dodge_entities_reset_meteors_for_level(&dodge);

    dodge.state = STATE_PLAYING;
}

static void start_new_game(GameData* data)
{
    reset_runtime_values(data);
    dodge_entities_hide_all(&dodge);
    dodge_audio_start();
    start_level(data);
}

static void start_next_level_transition(void)
{
    dodge_entities_hide_all(&dodge);

    if (dodge.level < LEVEL_MAX)
        dodge.level++;

    dodge.shield_active = 0;
    dodge.shield_spawn_timer = 0;
    dodge.invincible_timer = 0;
    dodge.transition_timer = TRANSITION_FRAMES;
    dodge.transition_redraw_timer = 0;

    dodge_audio_level_up();
    dodge_ui_draw_star_transition(&dodge);
    dodge.state = STATE_LEVEL_TRANSITION;
}

static void end_game(GameData* data)
{
    dodge.state = STATE_END_SCREEN;

    dodge_audio_hit();
    dodge_save_high_score(data);
    dodge_entities_hide_all(&dodge);
    HIDE_WIN;
    dodge_ui_draw_end_screen(data);
}

static void win_game(GameData* data)
{
    dodge.state = STATE_WIN_SCREEN;

    dodge_audio_level_up();
    dodge_save_high_score(data);
    dodge_entities_hide_all(&dodge);
    HIDE_WIN;
    dodge_ui_draw_win_screen(data);
}

void dodge_blocks_init(GameData* data)
{
    dodge.state = STATE_READY;
    reset_runtime_values(data);
    HIDE_SPRITES;
    HIDE_WIN;
    dodge_audio_init();

    load_sprite_tiles();
    clear_screen_layers();

    dodge_entities_init(&dodge);
    dodge_entities_hide_all(&dodge);
    dodge_ui_draw_begin_screen();
    SHOW_SPRITES;
}

static void update_ready(GameData* data)
{
    if (INPUT_PRESSED(PAD_A))
        start_new_game(data);
}

static void update_end_screen(GameData* data)
{
    if (INPUT_PRESSED(PAD_A))
        start_new_game(data);
}

static void update_win_screen(GameData* data)
{
    if (INPUT_PRESSED(PAD_A))
        start_new_game(data);
}

static void update_score_and_level(GameData* data)
{
    dodge.score_frame++;
    dodge.timer_frame++;
    dodge.level_timer++;

    if (dodge.score_frame >= 30) {
        dodge.score_frame = 0;
        data->score++;
        dodge_ui_draw_score(data);
    }

    if (dodge.timer_frame >= 60) {
        dodge.timer_frame = 0;
        dodge_ui_draw_timer(&dodge);
    }

    if (dodge.level_timer >= LEVEL_CHANGE_FRAMES) {
        dodge.level_timer = 0;
        if (dodge.level < LEVEL_MAX) {
            start_next_level_transition();
        } else {
            win_game(data);
        }
    }
}

static void pause_game(void)
{
    dodge_entities_hide_visual(&dodge);
    dodge_ui_draw_pause_screen();
    dodge.state = STATE_PAUSED;
}

static void resume_game(GameData* data)
{
    dodge_ui_draw_clean_background(&dodge, data);
    dodge_entities_show_active(&dodge);
    dodge.state = STATE_PLAYING;
}

static void update_pause(GameData* data)
{
    if (INPUT_PRESSED(PAD_A))
        resume_game(data);
}

static void update_playing(GameData* data)
{
    if (INPUT_PRESSED(PAD_START)) {
        pause_game();
        return;
    }

    dodge_entities_update_player(&dodge);
    update_score_and_level(data);

    if (dodge.state != STATE_PLAYING)
        return;

    dodge_entities_update_meteor_animation(&dodge);

    if (dodge_entities_update_meteors(&dodge)) {
        end_game(data);
        return;
    }

    dodge_entities_update_shield_powerup(&dodge);

    if (INPUT_PRESSED(PAD_SELECT))
        end_game(data);
}

static void update_level_transition(GameData* data)
{
    if (dodge.transition_timer > 0) {
        dodge.transition_timer--;
        dodge.transition_redraw_timer++;

        if (dodge.transition_redraw_timer >= 8) {
            dodge.transition_redraw_timer = 0;
            dodge_ui_draw_star_transition(&dodge);
        }
        return;
    }
    start_level(data);
}

void dodge_blocks_game(GameData* data)
{
    switch (dodge.state) {
        case STATE_READY:
            update_ready(data);
            break;

        case STATE_PLAYING:
            update_playing(data);
            break;

        case STATE_LEVEL_TRANSITION:
            update_level_transition(data);
            break;

        case STATE_END_SCREEN:
            update_end_screen(data);
            break;

        case STATE_WIN_SCREEN:
            update_win_screen(data);
            break;

        case STATE_PAUSED:
            update_pause(data);
            break;

        default:
            break;
    }
}
