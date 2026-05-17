  .module battle_math
  .area _CODE

  .globl _compute_damage_asm

  ; uint8_t compute_damage_asm(uint8_t power, uint8_t attack, uint8_t defense)
_compute_damage_asm:
  ld hl, #2
  add hl, sp

  ld b, (hl)
  inc hl
  ld c, (hl)
  inc hl
  ld e, (hl)

  ld a, e
  or a
  jr z, _max_damage

  ld hl, #0
  ld a, #8
  ld d, a
_mult_loop:
  srl b
  jr nc, _mult_skip
  add hl, bc
_mult_skip:
  sla c
  dec d
  jr nz, _mult_loop

  ld a, l
  ld d, h
  ld e, a
  ld a, (hl)
  ld c, #0
  ld a, e

_div_loop:

_max_damage:
  ld e, #255
  ret
