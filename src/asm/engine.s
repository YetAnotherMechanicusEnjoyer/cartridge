  .module engine
  .area _CODE

  .globl _init_window_layer
  .globl _lcd_isr
  .globl _scroll_x_val

  ; void init_window_layer(void)
_init_window_layer:
  ; WX register => 0xFF4B
  ; WY register => 0xFF4A

  LD A, #7
  LD (#0xFF4B), A ; WX = 7

  LD A, #128
  LD (#0xFF4A), A ; WY = 128
  RET

  ; void lcd_isr(void)
_lcd_isr:
  ; SCX register => 0xFF43
  PUSH AF
  PUSH HL

  LD HL, #_scroll_x_val
  LD A, (HL)
  LD (#0xFF43), A

  POP HL
  POP AF
  RET
