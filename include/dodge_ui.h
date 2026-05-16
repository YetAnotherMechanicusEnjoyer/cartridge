#ifndef DODGE_UI_H
#define DODGE_UI_H

#include "game.h"
#include "dodge_type.h"

void dodge_ui_draw_begin_screen(void);
void dodge_ui_draw_pause_screen(void);
void dodge_ui_draw_end_screen(GameData* data);
void dodge_ui_draw_win_screen(GameData* data);

void dodge_ui_draw_clean_background(DodgeState* dodge, GameData* data);
void dodge_ui_draw_score(GameData* data);
void dodge_ui_draw_timer(DodgeState* dodge);
void dodge_ui_draw_shield_status(DodgeState* dodge);
void dodge_ui_draw_star_transition(DodgeState* dodge);

#endif
