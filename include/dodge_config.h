#ifndef DODGE_CONFIG_H
#define DODGE_CONFIG_H

#define PLAYER_SPRITE          0
#define FIRST_METEOR_SPRITE    1
#define SHIELD_SPRITE          6
#define ZIGZAG_START_LEVEL      3
#define ZIGZAG_METEOR_INDEX     0


#define TILE_BLANK             0
#define TILE_PLAYER            1
#define TILE_PLAYER_BOOST      2
#define TILE_METEORITE         3
#define TILE_METEORITE2        4
#define TILE_SHIELD            5

#define PLAYER_Y               136
#define PLAYER_MIN_X           16
#define PLAYER_MAX_X           152
#define PLAYER_SPEED           2

#define METEOR_COUNT           5

#define STATE_READY             0
#define STATE_PLAYING           1
#define STATE_LEVEL_TRANSITION  2
#define STATE_END_SCREEN        3
#define STATE_WIN_SCREEN        4
#define STATE_PAUSED            5

#define LEVEL_MAX               10
#define LEVEL_CHANGE_FRAMES     1800
#define TRANSITION_FRAMES       150
#define SHIELD_SPAWN_FRAMES     600

#define STAR_SPRITE            7
#define TILE_STAR              6
#define STAR_SPAWN_FRAMES      420
#define STAR_SCORE_BONUS       5

#endif
