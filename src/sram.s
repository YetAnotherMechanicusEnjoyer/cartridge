  .module sram
  .area _CODE

  .globl _save_score
  .globl _load_score

  ; void save_score(uint8_t score)
_save_score:
  LD E, A

  LD A, #0x0A
  LD (#0x0000), A

  XOR A
  LD (#0x4000), A

  LD A, E
  LD (#0xA000), A

  XOR A
  LD (#0x0000), A
  RET

  ; uint8_t load_score(void)
_load_score:
  LD A, #0x0A
  LD (#0x0000), A

  XOR A
  LD (#0x4000), A

  LD A, (#0xA000)
  LD E, A
  PUSH AF

  XOR A
  LD (#0x0000), A

  POP AF
  RET
