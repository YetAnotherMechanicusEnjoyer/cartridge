  .module sram
  .area _CODE

  .globl _save_score
  .globl _load_score

  ; void save_score(uint16_t score)
_save_score:
  pop bc
  pop de
  push de
  push bc

  ; 0x0a = sram
  ld a, #0x0a
  ld (#0x0000), a

  xor a
  ld (#0x4000), a

  ld a, e
  ld (#0xa000), a

  ld a, d
  ld (#0xa001), a

  xor a
  ld (#0x0000), a
  ret

  ; uint16_t load_score(void)
_load_score:
  ; 0x0a = sram
  ld a, #0x0a
  ld (#0x0000), a

  xor a
  ld (#0x4000), a

  ld a, (#0xa000)
  ld e, a

  ld a, (#0xa001)
  ld d, a

  xor a
  ld (#0x0000), a
  ret
