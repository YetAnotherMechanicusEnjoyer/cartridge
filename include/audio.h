#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

void init_sound(void);
void sfx_confirm(void);
void sfx_laser(void);
void sfx_hit(void);
void sfx_warp(void);
void play_terminal_click(uint8_t seed);

#endif /* AUDIO_H */
