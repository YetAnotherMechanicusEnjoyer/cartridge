#include "npc.h"
#include <gb/gb.h>

NPC npcs[MAX_NPC];

extern uint8_t get_collision_at(uint8_t x, uint8_t y);

void npc_process_logic(NPC* n) {
  if (!n->active) return;

  if (n->moving) {
    switch (n->direction) {
      case 0:
        n->y++;
        break;
      case 1:
        n->y--;
        break;
      case 2:
        n->x--;
        break;
      case 3:
        n->x++;
        break;
    }

    n->move_timer--;
    if (n->move_timer == 0) n->moving = 0;
    return;
  }

  if (n->wait_timer > 0) {
    n->wait_timer--;
    return;
  }

  uint8_t chance = DIV_REG & 0x0F;
  if (chance < 2) {
    n->direction = DIV_REG & 0x03;

    uint8_t next_x = n->x;
    uint8_t next_y = n->y;

    uint8_t distance = ((DIV_REG & 0x03) + 1) * 8;

    switch (n->direction) {
      case 3:
        next_x += distance;
        break;
      case 2:
        next_x -= distance;
        break;
      case 1:
        next_y -= distance;
        break;
      case 0:
        next_y += distance;
        break;
    }

    if (get_collision_at(next_x + 4, next_y + 4) != 1) {
      n->moving = 1;
      n->move_timer = distance;
    }

    n->wait_timer = 60 + (DIV_REG & 0x3F);
  }
}

void npcs_init(void) {
  for (uint8_t i = 0; i < MAX_NPC; i++) npcs[i].active = 0;
}

void npcs_render(uint8_t px, uint8_t py, uint8_t scx, uint8_t scy) {
  for (uint8_t i = 0; i < MAX_NPC; i++) {
    if (!npcs[i].active) {
      move_sprite(i + 1, 0, 0);
      continue;
    }

    uint8_t n_x = (npcs[i].x - scx) + 8;
    uint8_t n_y = (npcs[i].y - scy) + 16;

    move_sprite(i + 1, n_x, n_y);
    set_sprite_tile(i + 1, npcs[i].sprite_id);
  }
}
