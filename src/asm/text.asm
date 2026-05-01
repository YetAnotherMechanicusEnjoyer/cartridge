  .module text
  .area _CODE

  .globl _draw_string
  .globl _text_x, _text_y, _text_ptr

  .globl _clear_string
  .globl _text_len

  ; void draw_string(void)
_draw_string:
  ; 0x9c00 + (y * 32) + x => pixel addr
  ld a, (#_text_y)
  ld l, a
  ld h, #0
  add hl, hl ; *2
  add hl, hl ; *4
  add hl, hl ; *8
  add hl, hl ; *16
  add hl, hl ; *32

  ld a, (#_text_x)
  ld e, a
  ld d, #0
  add hl, de ; hl = (y * 32) + x

  ld de, #0x9c00
  add hl, de

  ld a, (#_text_ptr)
  ld e, a
  ld a, (#_text_ptr + 1)
  ld d, a

draw_loop:
  ld a, (de)
  or a
  ret z

  sub #32

  ld b, a

wait_vram_text:
  ldh a, (#0xff41)
  and #0x02
  jr nz, wait_vram_text

  ld (hl), b
  inc hl
  inc de
  jr draw_loop

  ; void clear_string(void)
_clear_string:
  ld a, (#_text_y)
  ld l, a
  ld h, #0
  add hl, hl ; *2
  add hl, hl ; *4
  add hl, hl ; *8
  add hl, hl ; *16
  add hl, hl ; *32

  ld a, (#_text_x)
  ld e, a
  ld d, #0
  add hl, de ; hl = (y * 32) + x

  ld de, #0x9c00
  add hl, de

  ld a, (#_text_len)
  ld c, a
  or a
  ret z

clear_loop:

wait_vram_clear:
  ldh a, (#0xff41)
  and #0x02
  jr nz, wait_vram_clear

  xor a
  ld (hl+), a

  dec c
  jr nz, clear_loop
  ret
