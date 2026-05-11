#ifndef TRADER_H
#define TRADER_H

#include "battle.h"

#define MAX_INVENTORY_SLOTS 8
#define MAX_ITEMS_IN_GAME 10
#define MAX_STATIONS 5
#define MAX_MAP_SIZE 1024
#define MAX_EVENTS 5

typedef struct {
  char headline[20];
  char description[40];
  uint8_t target_station_id;
  uint8_t target_item_id;
  uint8_t price_modifier;
} EventDef;

extern const EventDef event_registry[MAX_EVENTS];

typedef struct {
  char name[11];
  uint16_t base_price;
  uint8_t volatility;
} ItemDef;

typedef struct {
  char name[15];
  uint8_t tech_level;
  uint8_t supply_mods[MAX_ITEMS_IN_GAME];
  const uint8_t* map;
  uint8_t threat;
  char faction[8];
  uint8_t distance;
} StationDef;

typedef struct {
  uint8_t item_id;
  uint8_t quantity;
} InvSlot;

typedef struct {
  uint8_t save_initialized;
  uint32_t credits;
  uint8_t current_station_id;
  uint8_t market_seed;
  uint8_t current_event_id;
  InvSlot inventory[MAX_INVENTORY_SLOTS];
  BattleEntity player_ship;
} SaveData;

extern const ItemDef item_registry[MAX_ITEMS_IN_GAME];
extern const StationDef station_registry[MAX_STATIONS];

extern uint8_t fast_rng(uint8_t seed);

uint16_t get_market_price(uint8_t item_id, uint8_t station_id, uint8_t market_seed, uint8_t current_event_id);

#endif /* TRADER_H */
