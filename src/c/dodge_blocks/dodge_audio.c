#include "dodge_audio.h"

#include <gb/gb.h>
#include <stdint.h>

void dodge_audio_init(void)
{
    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xFF;
}

static void dodge_audio_beep(uint8_t low, uint8_t high)
{
    NR10_REG = 0x00;
    NR11_REG = 0x80;
    NR12_REG = 0xF3;
    NR13_REG = low;
    NR14_REG = 0x80 | high;
}

void dodge_audio_start(void)
{
    dodge_audio_beep(0xC0, 0x86);
}

void dodge_audio_level_up(void)
{
    dodge_audio_beep(0x80, 0x87);
}

void dodge_audio_shield(void)
{
    dodge_audio_beep(0x20, 0x87);
}

void dodge_audio_hit(void)
{
    NR41_REG = 0x3F;
    NR42_REG = 0xF2;
    NR43_REG = 0x55;
    NR44_REG = 0x80;
}
