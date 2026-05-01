  .module utoa
  .area _CODE

  .globl _utoa
  .globl _text_val, _text_ptr

  ; void utoa(void)
_utoa:
  ld a, (#_text_val)
  ld l, a
  ld a, (#_text_val + 1)
  ld h, a

  ld a, (#_text_ptr)
  ld e, a
  ld a, (#_text_ptr + 1)
  ld d, a

  ld bc, #0xd8f0 ; -10000
  call extract_digit

  ld bc, #0xfc18 ; -1000
  call extract_digit

  ld bc, #0xff9c ; -100
  call extract_digit

  ld bc, #0xfff6 ; -10
  call extract_digit

  ld a, l
  add a, #48
  ld (de), a
  inc de

  xor a
  ld (de), a
  ret

extract_digit:
  ld a, #47

subtract_loop:
  inc a
  add hl, bc
  jr c, subtract_loop
  push bc
  push af
  ld a, b
  cpl
  ld b, a
  ld a, c
  cpl
  ld c, a
  inc bc
  call c, carry
  pop af
  add hl, bc
  pop bc
  ld (de), a
  inc de
  ret

carry:
  dec bc
  ret
