#include "dodge_ui.h"
#include "dodge_config.h"
#include "dodge_utils.h"
#include "asm_wrapper.h"

#include <gb/gb.h>
#include <stdint.h>

static void dodge_ui_draw_value_line(uint8_t y, char* label, uint16_t value)
{
    char line[18];
    uint8_t i;
    uint8_t pos;

    pos = 0;
    i = 0;

    while (label[i]) {
        line[pos] = label[i];
        pos++;
        i++;
    }

    line[pos] = ' ';
    pos++;

    dodge_write_number_5(&line[pos], value);

    pos += 5;
    line[pos] = '\0';

    dodge_text_center_bg(y, line);
}

static void dodge_ui_draw_level_number(DodgeState* dodge)
{
    display_message_bg(0, 0, "LV ");

    if (dodge->level == 1) {
        display_message_bg(3, 0, "1 ");
    } else if (dodge->level == 2) {
        display_message_bg(3, 0, "2 ");
    } else if (dodge->level == 3) {
        display_message_bg(3, 0, "3 ");
    } else if (dodge->level == 4) {
        display_message_bg(3, 0, "4 ");
    } else if (dodge->level == 5) {
        display_message_bg(3, 0, "5 ");
    } else if (dodge->level == 6) {
        display_message_bg(3, 0, "6 ");
    } else if (dodge->level == 7) {
        display_message_bg(3, 0, "7 ");
    } else if (dodge->level == 8) {
        display_message_bg(3, 0, "8 ");
    } else if (dodge->level == 9) {
        display_message_bg(3, 0, "9 ");
    } else {
        display_message_bg(3, 0, "10");
    }
}

void dodge_ui_draw_timer(DodgeState* dodge)
{
    uint16_t remaining_frames;
    uint8_t seconds_left;
    char timer_text[5];

    if (dodge->level_timer >= LEVEL_CHANGE_FRAMES) {
        seconds_left = 0;
    } else {
        remaining_frames = LEVEL_CHANGE_FRAMES - dodge->level_timer;
        seconds_left = (remaining_frames + 59) / 60;
    }

    timer_text[0] = 'T';
    timer_text[1] = ' ';
    timer_text[2] = '0' + (seconds_left / 10);
    timer_text[3] = '0' + (seconds_left % 10);
    timer_text[4] = '\0';

    display_message_bg(15, 0, timer_text);
}

void dodge_ui_draw_score(GameData* data)
{
    char score_text[9];

    score_text[0] = 'S';
    score_text[1] = 'C';
    score_text[2] = ' ';

    dodge_write_number_5(&score_text[3], data->score);

    score_text[8] = '\0';

    display_message_bg(0, 1, score_text);
}

void dodge_ui_draw_shield_status(DodgeState* dodge)
{
    if (dodge->shield_active)
        display_message_bg(6, 0, "SH");
    else
        display_message_bg(6, 0, "  ");
}

static void dodge_ui_draw_status_line(DodgeState* dodge, GameData* data)
{
    dodge_ui_draw_level_number(dodge);
    dodge_ui_draw_shield_status(dodge);
    dodge_ui_draw_timer(dodge);
    dodge_ui_draw_score(data);
}

static void dodge_ui_draw_static_stars(void)
{
    display_message_bg(2, 4, "*");
    display_message_bg(9, 5, "*");
    display_message_bg(16, 3, "*");
    display_message_bg(5, 9, "*");
    display_message_bg(13, 10, "*");
    display_message_bg(18, 7, "*");
    display_message_bg(3, 14, "*");
    display_message_bg(11, 15, "*");
    display_message_bg(17, 13, "*");
}

void dodge_ui_draw_clean_background(DodgeState* dodge, GameData* data)
{
    fill_bkg_rect(0, 0, 32, 32, TILE_BLANK);
    dodge_ui_draw_static_stars();
    dodge_ui_draw_status_line(dodge, data);
}

void dodge_ui_draw_begin_screen(void)
{
    fill_bkg_rect(0, 0, 32, 32, TILE_BLANK);

    dodge_text_center_bg(2, "SPACE DODGE");
    dodge_text_center_bg(6, "PRESS ENTER");
    dodge_text_center_bg(7, "TO START");
}

void dodge_ui_draw_pause_screen(void)
{
    fill_bkg_rect(0, 0, 32, 32, TILE_BLANK);
    dodge_text_center_bg(5, "PAUSED");
    dodge_text_center_bg(8, "PRESS ENTER");
}

void dodge_ui_draw_end_screen(GameData* data)
{
    fill_bkg_rect(0, 0, 32, 32, TILE_BLANK);
    dodge_text_center_bg(2, "GAME OVER");
    dodge_ui_draw_value_line(5, "SCORE", data->score);
    dodge_text_center_bg(10, "PRESS ENTER");
    dodge_text_center_bg(11, "TO START AGAIN");
}

void dodge_ui_draw_win_screen(GameData* data)
{
    fill_bkg_rect(0, 0, 32, 32, TILE_BLANK);
    dodge_text_center_bg(2, "MISSION CLEAR");
    dodge_ui_draw_value_line(5, "SCORE", data->score);
    dodge_text_center_bg(10, "PRESS ENTER");
    dodge_text_center_bg(11, "TO PLAY AGAIN");
}

void dodge_ui_draw_star_transition(DodgeState* dodge)
{
    uint8_t i;
    uint8_t x;
    uint8_t y;

    fill_bkg_rect(0, 0, 32, 32, TILE_BLANK);

    for (i = 0; i < 24; i++) {
        x = dodge_rand8(dodge) % 20;
        y = 1 + (dodge_rand8(dodge) % 16);

        if (y >= 7 && y <= 9 && x >= 4 && x <= 15) {
            if (y < 8)
                y = 5;
            else
                y = 11;
        }
        display_message_bg(x, y, "*");
    }

    if (dodge->transition_timer & 16)
        dodge_text_center_bg(8, "LEVEL UP!");
}
