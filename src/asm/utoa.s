  .module utoa
  .area _CODE

  .globl _utoa
  .globl _text_val, _text_ptr

  ; void utoa(void)
_utoa:
  LD A, (#_text_val)
  LD L, A
  LD A, (#_text_val + 1)
  LD H, A

  LD A, (#_text_ptr)
  LD E, A
  LD A, (#_text_ptr + 1)
  LD D, A

  LD BC, #0xD8F0 ; -10000
  CALL extract_digit

  LD BC, #0xFC18 ; -1000
  CALL extract_digit

  LD BC, #0xFF9C ; -100
  CALL extract_digit

  LD BC, #0xFFF6 ; -10
  CALL extract_digit

  LD A, L
  ADD A, #48
  LD (DE), A
  INC DE

  XOR A
  LD (DE), A
  RET

extract_digit:
  LD A, #47

subtract_loop:
  INC A
  ADD HL, BC
  JR C, subtract_loop
  PUSH BC
  PUSH AF
  LD A, B
  CPL
  LD B, A
  LD A, C
  CPL
  LD C, A
  INC BC
  CALL C, carry
  POP AF
  ADD HL, BC
  POP BC
  LD (DE), A
  INC DE
  RET

carry:
  DEC BC
  RET
