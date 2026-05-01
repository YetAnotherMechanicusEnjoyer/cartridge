  .module timers
  ; RAM (BSS = 0)
  .area _BSS

  .globl _timer_counters
  .globl _timer_flags

_timer_counters::
  .ds 8 ; 8 Bytes array (255 frames per 1 Byte timer => ~4.2 secs)
_timer_flags::
  .ds 1 ; 1 Byte flag (bit n = n timer ended)

  .area _CODE
  .globl _timers_update

  ; void timers_update(void)
_timers_update:
  ld hl, #_timer_counters
  ld b, #8
  ld c, #1
  ld d, #0

update_loop:
  ld a, (hl)
  or a
  jr z, next_timer

  dec a
  ld (hl), a
  or a
  jr nz, next_timer

  ld a, d
  or c
  ld d, a

next_timer:
  inc hl
  sla c ; slide one bit to the left
  dec b
  jr nz, update_loop

  ld a, (#_timer_flags)
  or d
  ld (#_timer_flags), a

  ret
