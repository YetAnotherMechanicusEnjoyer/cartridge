#ifndef NPC_H
#define NPC_H

#include <stdint.h>

typedef struct {
  uint8_t x, y;
  uint8_t sprite_id;
  uint8_t moving;
  uint8_t move_timer;
  uint8_t direction;
  uint8_t wait_timer;
  uint8_t active;
} NPC;

#define MAX_NPC 4

extern NPC npcs[MAX_NPC];

void npc_process_logic(NPC* n);
void npcs_init(void);
void npcs_render(uint8_t scx, uint8_t scy);

#endif
