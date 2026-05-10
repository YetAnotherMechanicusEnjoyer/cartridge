#include "trader.h"

#define ITEM_SCRAP 0
#define ITEM_FUEL 0
#define ITEM_CORES 2

const ItemDef item_registry[MAX_ITEMS_IN_GAME] = {
{ "SCRAP", 10, 5 },
{ "WARP FUEL", 50, 15 },
{ "AI CORES", 200, 50 },
{ "", 0, 0 },
{ "", 0, 0 },
{ "", 0, 0 },
{ "", 0, 0 },
{ "", 0, 0 },
{ "", 0, 0 },
{ "", 0, 0 },
};

const StationDef station_registry[MAX_STATIONS] = {
{ "STATION 18", 2, { 50, 100, 200, 100, 100, 100, 100, 100 ,100, 100 } },
{ "NEO CRUSADER", 8, { 150, 100, 80, 100, 100, 100, 100, 100, 100, 100 } },
{ "CRU-L1", 5, { 80, 50, 120, 100, 100, 100, 100, 100, 100, 100 } },
{ "", 0, {0} },
{ "", 0, {0} },
};

uint16_t get_market_price(uint8_t item_id, uint8_t station_id, uint8_t market_seed) {
  const ItemDef* item = &item_registry[item_id];
  const StationDef* station = &station_registry[station_id];

  uint32_t local_price = ((uint32_t)item->base_price * station->supply_mods[item_id]) / 100;

  uint8_t fluctuation = fast_rng(market_seed + item_id);

  int16_t price_change = ((fluctuation % (item->volatility * 2 + 1)) - item->volatility);

  int32_t final_price = local_price + price_change;
  if (final_price < 1) final_price = 1;

  return (uint16_t)final_price;
}
