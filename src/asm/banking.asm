.module banking

.area _DATA

_rom_bank_current::
  .ds 1

.area _CODE

.globl _rom_bank_current
.globl _rom_switch_mbc5_asm
.globl _sram_bank_switch_asm

; ROM switch
_rom_switch_mbc5_asm::
  ldhl sp, #2
  ld a, (hl)
  ld (#_rom_bank_current), a
  ld (#0x2000), a
  xor a
  ld (#0x3000), a
  ret

; SRAM bank
_sram_bank_switch_asm::
  ldhl sp, #2
  ld a, (hl)
  and #0x0F
  ld (#0x4000), a
  ret
