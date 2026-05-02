  .module sram
  .area _CODE

  .globl _sram_write
  .globl _sram_read

  ; void sram_write(uint16_t offset, uint8_t* data, uint16_t length)
_sram_write::
  ; enable sram
  ld a, #0x0A
  ld (#0x0000), a

  ld hl, #2
  add hl, sp

  ; offset
  ld e, (hl)
  inc hl
  ld d, (hl)

  ; offset to sram addr
  ld a, d
  add a, #0xA0
  ld d, a

  inc hl
  ld c, (hl)
  inc hl
  ld b, (hl)

  inc hl
  ld a, (hl)
  inc hl
  ld h, (hl)
  ld l, a

write_loop:
  ; if hl length == 0
  ld a, h
  or l
  jr z, write_end

  ; ram (bc) to sram (de)
  ld a, (bc)
  ld (de), a

  inc bc
  inc de
  dec hl
  jr write_loop

write_end:
  ; disable sram
  ld a, #0x00
  ld (#0x0000), a
  ret

  ; void sram_read(uint16_t offset, uint8_t* data, uint16_t length)
_sram_read::
  ; enable sram
  ld a, #0x0A
  ld (#0x0000), a

  ld hl, #2
  add hl, sp

  ; offset
  ld e, (hl)
  inc hl
  ld d, (hl)
  ld a, d
  add a, #0xA0
  ld d, a

  inc hl
  ld c, (hl)
  inc hl
  ld b, (hl)

  inc hl
  ld a, (hl)
  inc hl
  ld h, (hl)
  ld l, a

read_loop:
  ; if hl length == 0
  ld a, h
  or l
  jr z, read_end

  ; sram (de) to ram (bc)
  ld a, (de)
  ld (bc), a

  inc bc
  inc de
  dec hl
  jr read_loop

read_end:
  ; disable sram
  ld a, #0x00
  ld (#0x0000), a
  ret
