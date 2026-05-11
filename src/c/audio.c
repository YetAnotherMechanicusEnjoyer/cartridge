#include "trader.h"
#include <gb/gb.h>

void init_sound(void) {
  NR52_REG = 0x80;
  NR51_REG = 0xFF;
  NR50_REG = 0x77;
}

void sfx_confirm(void) {
  NR10_REG = 0x00;
  NR11_REG = 0x81;
  NR12_REG = 0x43;
  NR13_REG = 0x73;
  NR14_REG = 0x86;
}

void sfx_laser(void) {
  NR10_REG = 0x1E;
  NR11_REG = 0x10;
  NR12_REG = 0xF3;
  NR13_REG = 0x00;
  NR14_REG = 0x87;
}

void sfx_hit(void) {
  NR41_REG = 0x1F;
  NR42_REG = 0xF1;
  NR43_REG = 0x53;
  NR44_REG = 0xC0;
}

void sfx_warp(void) {
  NR10_REG = 0x15;
  NR11_REG = 0x80;
  NR12_REG = 0xA4;
  NR13_REG = 0x00;
  NR14_REG = 0x85;
}

void play_terminal_click(uint8_t seed) {
  NR41_REG = 0x01;
  NR42_REG = 0x11;
  NR43_REG = 0x00 + (fast_rng(seed) & 0x0F);
  NR44_REG = 0xC0;
}

