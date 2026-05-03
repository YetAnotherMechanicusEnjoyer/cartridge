  .module collision
  .area _CODE

  .globl _check_collision
  .globl _col_x, _col_y
  .globl _background_map

  ; uint8_t check_collision(void)
_check_collision:
  ; Y - 16 (offset) + 4 (sprite center) = Y - 12
  ld a, (#_col_y)
  sub a, #12

  ; divide by 8
  srl a
  srl a
  srl a
  ld c, a

  ld a, c
  sla a
  sla a
  ld b, a
  sla a
  sla a
  add a, b
  ld l, a
  ld h, #0

  ; X - 8 (offset) + 4 (sprite center) = X - 4
  ld a, (#_col_x)
  sub a, #4

  srl a
  srl a
  srl a

  ld e, a
  ld d, #0
  add hl, de

  ld de, #_background_map
  add hl, de
  ld a, (hl)

  ; if tile >= 0x05 it is a wall
  cp a, #0x05
  jr c, is_free

is_solid:
  ld a, #1
  ret

is_free:
  xor a
  ret
