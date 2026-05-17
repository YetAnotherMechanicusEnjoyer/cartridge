#include "dodge_utils.h"
#include "dodge_config.h"
#include "asm_wrapper.h"

#include <stdint.h>

static uint8_t dodge_strlen8(char* str)
{
    uint8_t len;

    len = 0;
    while (str[len])
        len++;
    return len;
}

void dodge_text_center_bg(uint8_t y, char* str)
{
    uint8_t len;
    uint8_t x;

    len = dodge_strlen8(str);
    if (len > 20)
        len = 20;
    x = (20 - len) / 2;
    display_message_bg(x, y, str);
}

void dodge_write_number_5(char* dst, uint16_t value)
{
    if (value > 99999)
        value = 99999;

    dst[0] = '0' + (value / 10000);
    value %= 10000;

    dst[1] = '0' + (value / 1000);
    value %= 1000;

    dst[2] = '0' + (value / 100);
    value %= 100;

    dst[3] = '0' + (value / 10);
    dst[4] = '0' + (value % 10);
}

uint8_t dodge_rand8(DodgeState* dodge)
{
    dodge->seed ^= dodge->seed << 3;
    dodge->seed ^= dodge->seed >> 5;
    dodge->seed ^= dodge->seed << 1;

    if (dodge->seed == 0)
        dodge->seed = 1;

    return dodge->seed;
}

uint8_t dodge_random_lane_x(DodgeState* dodge)
{
    return PLAYER_MIN_X + (dodge_rand8(dodge) % 9) * 16;
}

uint8_t dodge_collide8(uint8_t ax, uint8_t ay, uint8_t bx, uint8_t by)
{
    return ax < bx + 8 &&
           ax + 8 > bx &&
           ay < by + 8 &&
           ay + 8 > by;
}
