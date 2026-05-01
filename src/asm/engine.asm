  .module engine
  .area _code

  .globl _init_window_layer
  .globl _lcd_isr
  .globl _scroll_x_val

  ; void init_window_layer(void)
_init_window_layer:
  ; wx register => 0xff4b
  ; wy register => 0xff4a

  ld a, #7
  ld (#0xff4b), a ; wx = 7

  ld a, #128
  ld (#0xff4a), a ; wy = 128
  ret

  ; void lcd_isr(void)
_lcd_isr:
  ; scx register => 0xff43
  push af
  push hl

  ld hl, #_scroll_x_val
  ld a, (hl)
  ld (#0xff43), a

  pop hl
  pop af
  ret
