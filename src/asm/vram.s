  .module vram
  .area _CODE

  .globl _asm_src
  .globl _asm_dest
  .globl _asm_size

  .globl _vram_copy

  ; void vram_copy(void)
_vram_copy:
  LD HL, #_asm_size
  LD C, (HL)
  INC HL
  LD B, (HL)

  LD HL, #_asm_dest
  LD E, (HL)
  INC HL
  LD D, (HL)

  LD HL, #_asm_src
  LD A, (HL)
  INC HL
  LD H, (HL)
  LD L, A

copy_loop:

wait_stat:
  ; STAT register => 0xFF41
  LD A, (#0xFF41)
  AND #0x02
  JR NZ, wait_stat

  LD A, (HL)
  LD (DE), A
  INC HL
  INC DE

  DEC BC

  LD A, B
  OR C
  JR NZ, copy_loop

  RET
