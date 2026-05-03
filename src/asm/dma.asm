  .module dma

  .area _HRAM
_hram_dma_routine:: .ds 10

  .area _CODE
  .globl _init_dma
  .globl _execute_dma

dma_code_start:
  ld a, c
  ldh (#0x46), a
  ld a, #40
dma_wait:
  dec a
  jr nz, dma_wait
  ret
dma_code_end:

  ; void init_dma(void)
_init_dma:
  ld hl, #dma_code_start
  ld c, #0x80
  ld b, #(dma_code_end - dma_code_start)
copy_loop:
  ld a, (hl+)
  ldh (c), a
  inc c
  dec b
  jr nz, copy_loop
  ret

  ; void execute_dma(uint8_t hight_byte)
_execute_dma:
  di
  ldhl sp, #2
  ld c, (hl)
  call 0xFF80 ; HRAM
  ei
  ret
