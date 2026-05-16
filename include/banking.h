#ifndef BANKING_H
    #define BANKING_H
    #include <stdint.h>

#define BANK_FIXED       0
#define BANK_TEST_GAME   1
#define BANK_STATION     2
#define BANK_BATTLE      3
#define BANK_ECONOMY     4
#define BANK_BACKGROUND  5
#define BANK_SPRITES     6
#define BANK_NEW_GAME    7

extern uint8_t rom_bank_current;

extern void rom_switch_mbc5_asm(uint8_t bank) __sdcccall(0);
extern void sram_bank_switch_asm(uint8_t bank) __sdcccall(0);

/* ROM switch */
static inline void rom_switch(uint8_t bank)
{
  rom_switch_mbc5_asm(bank);
}

/* SRAM switch */
static inline void sram_switch(uint8_t bank)
{
  sram_bank_switch_asm(bank);
}

#endif
