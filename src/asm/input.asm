  .module input

  .area _BSS
  .globl _pad_current, _pad_previous, _pad_pressed, _pad_released

_pad_current::  .ds 1
_pad_previous:: .ds 1
_pad_pressed::  .ds 1
_pad_released:: .ds 1

  .area _CODE
  .globl _input_update

_input_update:
  ld a, (#_pad_current)
  ld (#_pad_previous), a
  ld c, a

  ; D-PAD
  ld a, #0x20
  ld (#0xFF00), a
  .rept 3
    ld a, (#0xFF00) ; prevent hardware bounce
  .endm
  cpl
  and #0x0F
  swap a
  ld b, a

  ; buttons
  ld a, #0x10
  ld (#0xFF00), a
  .rept 3
    ld a, (#0xFF00) ; prevent hardware bounce
  .endm
  cpl
  and #0x0F
  or b
  ld b, a

  ; current state
  ld (#_pad_current), a

  ; pressed state
  ld a, c
  cpl
  and b
  ld (#_pad_pressed), a

  ; released state
  ld a, b
  cpl
  and c
  ld (#_pad_released), a

  ; reset
  ld a, #0x30
  ld (#0xFF00), a
  ret
