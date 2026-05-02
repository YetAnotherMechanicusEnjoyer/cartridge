#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define PAD_DOWN   0x80
#define PAD_UP     0x40
#define PAD_LEFT   0x20
#define PAD_RIGHT  0x10
#define PAD_START  0x08
#define PAD_SELECT 0x04
#define PAD_B      0x02
#define PAD_A      0x01

extern uint8_t pad_previous;
extern uint8_t pad_current;
extern uint8_t pad_pressed;
extern uint8_t pad_released;
extern void input_update(void);

#define INPUT_HELD(button) (pad_current & (button))
#define INPUT_PRESSED(button) (pad_pressed & (button))
#define INPUT_RELEASED(button) (pad_released & (button))

#endif // !INPUT_H
