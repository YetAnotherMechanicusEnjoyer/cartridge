  .module hitbox
  .area _CODE

  .globl _get_hitbox_value
  .globl _hitbox_x, _hitbox_y
  .globl _collision_map

  ; uint8_t get_hitbox_value(void)
_get_hitbox_value:
  ld a, (#_hitbox_y)
  srl a
  srl a
  srl a
  ld l, a
  ld h, #0

  add hl, hl
  add hl, hl
  add hl, hl
  add hl, hl
  add hl, hl

  ld a, (#_hitbox_x)
  srl a
  srl a
  srl a
  ld e, a
  ld d, #0

  add hl, de

  ld de, #_collision_map
  add hl, de
  ld a, (hl)

  ld e, a
  ret
