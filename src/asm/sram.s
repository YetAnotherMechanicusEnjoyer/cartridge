  .module sram
  .area _CODE

  .globl _save_score
  .globl _load_score

  ; void save_score(uint16_t score)
_save_score:
  ; 0x0A = SRAM
  LD E, A

  LD A, #0x0A
  LD (#0x0000), A

  XOR A
  LD (#0x4000), A

  LD A, E
  LD (#0xA000), A

  LD A, D
  LD (#0xA001), A

  XOR A
  LD (#0x0000), A
  RET

  ; uint16_t load_score(void)
_load_score:
  ; 0x0A = SRAM
  LD A, #0x0A
  LD (#0x0000), A

  XOR A
  LD (#0x4000), A

  LD A, (#0xA000)
  LD E, A

  LD A, (#0xA001)
  LD D, A

  XOR A
  LD (#0x0000), A
  RET
