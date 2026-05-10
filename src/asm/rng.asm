  .module economy_path
  .area _CODE

  .globl _fast_rng

  ; uint8_t fast_rng(uint8_t seed)
_fast_rng:
  ld b, a
  rla
  rla
  rla
  rla
  rla
  rla
  rla
  xor b

  ld b, a
  srl a
  srl a
  srl a
  srl a
  srl a
  xor b

  ld b, a
  sla a
  sla a
  sla a
  xor b

  ret
