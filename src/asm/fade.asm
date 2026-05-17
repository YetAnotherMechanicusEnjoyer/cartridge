  .module fade
  .area _CODE

  .globl _fade_out_black
  .globl _fade_in_black

wait_frames:
1$:
  ldh a, (#0x44)
  cp #144
  jr c, 1$

2$:
  ldh a, (#0x44)
  cp #144
  jr nc, 2$

  dec b
  jr nz, 1$
  ret

apply_palette:
  ldh (#0x47), a
  ldh (#0x48), a
  ldh (#0x49), a

  ld b, #4
  call wait_frames
  ret

  ; void fade_out_black(void)
_fade_out_black:
  ld a, #0xE4
  call apply_palette
  ld a, #0xF9
  call apply_palette
  ld a, #0xFE
  call apply_palette
  ld a, #0xFF
  call apply_palette
  ret

  ; void fade_in_black(void)
_fade_in_black: 
  ld a, #0xFF
  call apply_palette
  ld a, #0xFE
  call apply_palette
  ld a, #0xF9
  call apply_palette
  ld a, #0xE4
  call apply_palette
  ret
