  .module text
  .area _CODE

  .globl _draw_string
  .globl _text_x, _text_y, _text_ptr

  .globl _clear_string
  .globl _text_len

  ; void draw_string(void)
_draw_string:
  ; 0x9C00 + (Y * 32) + X => pixel addr
  LD A, (#_text_y)
  LD L, A
  LD H, #0
  ADD HL, HL ; *2
  ADD HL, HL ; *4
  ADD HL, HL ; *8
  ADD HL, HL ; *16
  ADD HL, HL ; *32

  LD A, (#_text_x)
  LD E, A
  LD D, #0
  ADD HL, DE ; HL = (Y * 32) + X

  LD DE, #0x9C00
  ADD HL, DE

  LD A, (#_text_ptr)
  LD E, A
  LD A, (#_text_ptr + 1)
  LD D, A

draw_loop:
  LD A, (DE)
  OR A
  RET Z

  SUB #32

  LD B, A

wait_vram_text:
  LDH A, (#0xFF41)
  AND #0x02
  JR NZ, wait_vram_text

  LD (HL), B
  INC HL
  INC DE
  JR draw_loop

  ; void clear_string(void)
_clear_string:
  LD A, (#_text_y)
  LD L, A
  LD H, #0
  ADD HL, HL ; *2
  ADD HL, HL ; *4
  ADD HL, HL ; *8
  ADD HL, HL ; *16
  ADD HL, HL ; *32

  LD A, (#_text_x)
  LD E, A
  LD D, #0
  ADD HL, DE ; HL = (Y * 32) + X

  LD DE, #0x9C00
  ADD HL, DE

  LD A, (#_text_len)
  LD C, A
  OR A
  RET Z

clear_loop:

wait_vram_clear:
  LDH A, (#0xFF41)
  AND #0x02
  JR NZ, wait_vram_clear

  XOR A
  LD (HL+), A

  DEC C
  JR NZ, clear_loop
  RET
