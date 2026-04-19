  .module math_asm
  .area _CODE

  .globl _add_asm

_add_asm:
  ADD A, E
  LD E, A
  RET
