  .module vram
  .area _CODE

  .globl _asm_src
  .globl _asm_dest
  .globl _asm_size

  .globl _vram_copy

  ; void vram_copy(void)
_vram_copy:
  ld hl, #_asm_size
  ld c, (hl)
  inc hl
  ld b, (hl)

  ld hl, #_asm_dest
  ld e, (hl)
  inc hl
  ld d, (hl)

  ld hl, #_asm_src
  ld a, (hl)
  inc hl
  ld h, (hl)
  ld l, a

copy_loop:

wait_stat:
  ; stat register => 0xff41
  ld a, (#0xff41)
  and #0x02
  jr nz, wait_stat

  ld a, (hl)
  ld (de), a
  inc hl
  inc de

  dec bc

  ld a, b
  or c
  jr nz, copy_loop

  ret

_clear_window_asm:
  ld hl, #0x9C00
  ld bc, #80
  ld a, #0x00
_clear_loop:
  ld (hl+), a
  dec bc
  ld a, b
  or c
  ld a, #0x00
  jr nz, _clear_loop
  ret
