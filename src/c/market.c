#include "audio.h"
#include "market.h"
#include "asm.h"
#include "asm_wrapper.h"
#include "input.h"
#include "dialog.h"
#include "sprites.h"
#include "trader.h"
#include "sram.h"
#include <gb/gb.h>
#include <stdio.h>
#include <string.h>

#define M_WALKING 0
#define M_BUYING 1
#define M_SELLING 2

#define FEET_OFFSET_X 4
#define FEET_OFFSET_Y 0

#define EMPTY 0
#define WALL 1
#define BUY 2
#define SELL 3
#define EXIT 4
#define NEWS 5

typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
  uint8_t type;
} Trigger;

#define ACT_BUY 1
#define ACT_SELL 2
#define ACT_EXIT 3

#define NUM_TRIGGERS 3
const Trigger market_triggers[NUM_TRIGGERS] = {
{ 6, 7, 24, 16, ACT_BUY },
{ 10, 7, 24, 16, ACT_SELL },
{ 8, 17, 32, 16, ACT_EXIT },
};

const unsigned char market_tiles_graphics[] = {
  // 0
  0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  // 1
  0xFF,0xFF,0x80,0x80,0xBF,0xBF,0x80,0x80,0xFB,0xFB,0x80,0x80,0x80,0x80,0xFF,0xFF,
  // 2
  0x00,0x00,0x10,0x10,0x10,0x10,0x7C,0x7C,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x00,
  // 3
  0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  // 4
  0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xDB,0xDB,0x7E,0x7E,0x3C,0x3C,0x18,0x18,
  // 5
  0x00,0x00,0x00,0x00,0x00,0xBF,0x7C,0x7C,0xBF,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


static uint8_t market_sub_state = M_WALKING;
static uint8_t menu_cursor = 0;
static uint8_t menu_max_items = 0;
static uint8_t menu_items_map[MAX_ITEMS_IN_GAME];

uint8_t scroll_x = 0;
uint8_t scroll_y = 0;

#define NEWS_CONTENT_X 2
#define NEWS_CONTENT_Y 12
#define NEWS_MAX_WIDTH 17
#define NEWS_TYPE_SPEED 3

void read_news_terminal(GameData* data) {
  SaveData* save = data->current_save;
  uint8_t event_id = save->current_event_id;

  if (event_id >= MAX_EVENTS) event_id = 0;

  const EventDef* current_event = &event_registry[event_id];
  const char* text = current_event->description;

  HIDE_SPRITES;
  move_bkg(0, 0);
  clear_bg();
  char buf[21];

  for(uint8_t i = 0; i < 15; i++) {
    sprintf(buf, "DECRYPTING... [%u%%]", (i * 100) / 15);
    display_message_bg(0, 8, buf);
    if (i % 3 == 0) scroll_bkg(fast_rng(save->market_seed) % 4, fast_rng(save->market_seed) % 2);
    vsync();
    delay(50);
  }
  move_bkg(0, 0);
  clear_bg();

  display_message_bg(0, 0, ".------------------.");
  display_message_bg(0, 1, "|  GNN  LIVE FEED  |");
  display_message_bg(0, 2, "|------------------|");

  display_message_bg(1, 4, "       _|_       ");
  display_message_bg(1, 5, "      ( o )      ");
  display_message_bg(1, 6, "     /--^--\\     ");

  display_message_bg(0, 8, "|==================|");

  display_message_bg(1, 10, current_event->headline);

  display_message_bg(0, 16, "|__________________|");
  display_message_bg(2, 17, "PRESS [B] TO EXIT");

  uint16_t char_ptr = 0;
  uint8_t cur_x = NEWS_CONTENT_X;
  uint8_t cur_y = NEWS_CONTENT_Y;
  uint8_t timer = 0;
  uint8_t is_typing = 1;

  while(1) {
    input_update();
    if (!is_typing && INPUT_PRESSED(PAD_B)) {
      sfx_confirm();
      break;
    }

    timer++;
    uint8_t speed = (INPUT_HELD(PAD_A)) ? 1 : NEWS_TYPE_SPEED;

    if (is_typing && timer >= speed) {
      timer = 0;
      char c = text[char_ptr];

      if (c == '\0') {
        is_typing = 0;
      } else {
        if (c == ' ') {
          uint16_t temp_ptr = char_ptr + 1;
          uint8_t word_len = 0;
          while(text[temp_ptr] != ' ' && text[temp_ptr] != '\0') {
            word_len++;
            temp_ptr++;
          }
          if (cur_x + word_len > (NEWS_CONTENT_X + NEWS_MAX_WIDTH)) {
            cur_x = NEWS_CONTENT_X;
            cur_y++;
            char_ptr++;
            c = text[char_ptr];
          }
        } else {
          play_terminal_click(save->market_seed);
        }
        if (c != '\0') {
          char str[2] = {c, '\0'};
          display_message_bg(cur_x, cur_y, str);
          cur_x++;
          char_ptr++;
        }
        if (cur_x >= (NEWS_CONTENT_X + NEWS_MAX_WIDTH)) {
          cur_x = NEWS_CONTENT_X;
          cur_y++;
        }
      }
    }

    uint8_t frame_count = sys_time;
    if (frame_count % 30 == 0) {
      scroll_bkg(1, 0);
    } else if (frame_count % 31 == 0) {
      scroll_bkg(-1, 0);
    } else {
      move_bkg(0, 0);
    }

    if (frame_count % 60 < 30) {
      display_message_bg(14, 3, "LIVE");
    } else {
      display_message_bg(14, 3, "    ");
    }

    if (!is_typing) {
      if ((frame_count % 64) < 32) display_message_bg(cur_x, cur_y, " ");
      else display_message_bg(cur_x, cur_y, "_");
    }

    vsync();
  }
  move_bkg(0, 0);
  clear_bg();
  set_bkg_based_tiles(0, 0, 32, 32, station_registry[data->current_save->current_station_id].map, BKG_TILES_OFFSET);
  SHOW_SPRITES;
}

void draw_market_box(uint8_t w, uint8_t h) {
  clear_message_win(0, 0, 20, h);

  char border[22];
  border[0] = '+';
  for(uint8_t i = 1; i < w - 1; i++) border[i] = '=';
  border[w - 1] = '+';
  border[w] = '\0';

  display_message(0, 0, border);
  for(uint8_t i = 1; i < h - 1; i++) {
    display_message(0, i, "|");
    display_message(w - 1, i, "|");
  }
  display_message(0, h - 1, border);
}

void format_menu_line(char* out_buf, const char* name, uint16_t price, uint8_t qty) {
  char tmp[10];
  uint8_t idx = 1;

  out_buf[0] = ' ';

  if (qty > 0) {
    sprintf(tmp, "%dx", qty);
    for(uint8_t i = 0; tmp[i] != '\0' && idx < 5; i++) out_buf[idx++] = tmp[i];
  }

  for(uint8_t i = 0; name[i] != '\0' && i < 11; i++) out_buf[idx++] = name[i];

  while(idx < 14) out_buf[idx++] = ' ';

  sprintf(tmp, "%uC", price);
  for(uint8_t i = 0; tmp[i] != '\0' && idx < 18; i++) out_buf[idx++] = tmp[i];
  out_buf[idx] = '\0';
}

void open_buy_menu(GameData* data) {
  market_sub_state = M_BUYING;
  menu_cursor = 0;
  menu_max_items = 0;

  for(uint8_t i = 0; i < MAX_ITEMS_IN_GAME; i++) {
    if (item_registry[i].base_price > 0) {
      menu_items_map[menu_max_items] = i;
      menu_max_items++;
    }
  }

  uint8_t height = menu_max_items + 2;
  draw_market_box(20, height);

  for(uint8_t i = 0; i < menu_max_items; i++) {
    uint8_t item_id = menu_items_map[i];
    uint16_t price = get_market_price(item_id, data->current_save->current_station_id, data->current_save->market_seed, data->current_save->current_event_id);

    char line[20];
    format_menu_line(line, item_registry[item_id].name, price, 0);
    display_message(1, i + 1, line);
  }

  display_message(1, 1, ">");
  move_win(7, 144 - (height * 8));
}

void open_sell_menu(GameData* data) {
  menu_max_items = 0;

  for(uint8_t i = 0; i < MAX_INVENTORY_SLOTS; i++) {
    if (data->current_save->inventory[i].quantity > 0) {
      menu_items_map[menu_max_items] = i;
      menu_max_items++;
    }
  }

  if (menu_max_items == 0) {
    dialog_start("You have nothing to sell!");
    return;
  }

  market_sub_state = M_SELLING;
  menu_cursor = 0;

  uint8_t height = menu_max_items + 2;
  draw_market_box(20, height);

  for(uint8_t i = 0; i < menu_max_items; i++) {
    uint8_t slot = menu_items_map[i];
    uint8_t item_id = data->current_save->inventory[slot].item_id;
    uint8_t qty = data->current_save->inventory[slot].quantity;

    uint16_t price = get_market_price(item_id, data->current_save->current_station_id, data->current_save->market_seed, data->current_save->current_event_id);

    char line[20];
    format_menu_line(line, item_registry[item_id].name, price, qty);
    display_message(1, i + 1, line);
  }

  display_message(1, 1, ">");
  move_win(7, 144 - (height * 8));
}

void market_init(GameData* data) {
  data->state = MARKET;

  data->player_x = 80;
  data->player_y = 120;

  market_sub_state = M_WALKING;

  set_bkg_data(BKG_TILES_OFFSET, 5, market_tiles_graphics);
  set_bkg_based_tiles(0, 0, 32, 32, station_registry[data->current_save->current_station_id].map, BKG_TILES_OFFSET);

  /*for (uint8_t i = 0; i < NUM_TRIGGERS; i++) {
    display_message_bg(market_triggers[i].x, market_triggers[i].y, market_triggers[i].buf);
  }*/

  set_sprite_tile(0, 0);
  move_sprite(0, data->player_x, data->player_y);

  scroll_x = 0;
  scroll_y = 0;
  apply_scroll();

  SHOW_BKG;
  SHOW_SPRITES;
}

void market_state(GameData* data) {
  if (dialog_is_active()) {
    dialog_update();
    return;
  }

  static uint8_t draw = 1;

  if (draw) {
    display_message(1, menu_cursor + 1, ">");
    if (market_sub_state == M_BUYING) {
      open_buy_menu(data);
    } else if (market_sub_state == M_SELLING) {
      open_sell_menu(data);
    }
    draw = 0;
  }

  if (market_sub_state == M_BUYING || market_sub_state == M_SELLING) {
    if (INPUT_PRESSED(PAD_UP)) {
      display_message(1, menu_cursor + 1, " ");
      menu_cursor = (menu_cursor == 0) ? menu_max_items - 1 : menu_cursor - 1;
      display_message(1, menu_cursor + 1, ">");
    }

    if (INPUT_PRESSED(PAD_DOWN)) {
      display_message(1, menu_cursor + 1, " ");
      menu_cursor = (menu_cursor == menu_max_items - 1) ? 0 : menu_cursor + 1;
      display_message(1, menu_cursor + 1, ">");
    }

    if (INPUT_PRESSED(PAD_B)) {
      sfx_confirm();
      move_win(7, 144);
      market_sub_state = M_WALKING;
      dialog_start("Pleasure doing business!");
    }

    if (INPUT_PRESSED(PAD_A)) {
      sfx_confirm();
      draw = 1;
      if (market_sub_state == M_BUYING) {
        uint8_t item_id = menu_items_map[menu_cursor];
        uint16_t price = get_market_price(item_id, data->current_save->current_station_id, data->current_save->market_seed, data->current_save->current_event_id);

        if (data->current_save->credits >= price) {
          int8_t free_slot = -1;
          for(uint8_t i = 0; i < MAX_INVENTORY_SLOTS; i++) {
            if (data->current_save->inventory[i].quantity > 0 && data->current_save->inventory[i].item_id == item_id) {
              free_slot = i; break;
            } else if (data->current_save->inventory[i].quantity == 0 && free_slot == -1) {
              free_slot = i;
            }
          }

          if (free_slot != -1) {
            data->current_save->credits -= price;
            data->current_save->inventory[free_slot].item_id = item_id;
            data->current_save->inventory[free_slot].quantity++;
            sram_write(0, (uint8_t*)data->current_save, sizeof(SaveData));

            dialog_start("Item bought!");
          } else {
            dialog_start("Inventory full!");
          }
        } else {
          dialog_start("Not enough credits!");
        }
      } else if (market_sub_state == M_SELLING) {
        uint8_t slot = menu_items_map[menu_cursor];
        uint8_t item_id = data->current_save->inventory[slot].item_id;
        uint16_t price = get_market_price(item_id, data->current_save->current_station_id, data->current_save->market_seed, data->current_save->current_event_id);

        data->current_save->credits += price;
        data->current_save->inventory[slot].quantity--;
        sram_write(0, (uint8_t*)data->current_save, sizeof(SaveData));

        dialog_start("Item sold!");
      }
    }
    return;
  }

  uint8_t old_x = data->player_x;
  uint8_t old_y = data->player_y;

  uint8_t next_x = data->player_x;
  uint8_t next_y = data->player_y;

  if (INPUT_HELD(PAD_UP) && data->player_y > 0) next_y -= 1;
  if (INPUT_HELD(PAD_DOWN) && data->player_y < 255) next_y += 1;
  if (INPUT_HELD(PAD_LEFT) && data->player_x > 0) next_x -= 1;
  if (INPUT_HELD(PAD_RIGHT) && data->player_x < 255) next_x += 1;

  uint8_t terrain_type = get_collision_at(next_x + FEET_OFFSET_X, next_y + FEET_OFFSET_Y, station_registry[data->current_save->current_station_id].map);

  if (terrain_type != WALL) {
    data->player_x = next_x;
    data->player_y = next_y;
  }

  if (INPUT_PRESSED(PAD_A)) {
    switch (terrain_type) {
      case BUY:
        sfx_confirm();
        open_buy_menu(data);
        break;

      case SELL:
        sfx_confirm();
        open_sell_menu(data);
        break;

      case EXIT:
        sfx_confirm();
        HIDE_SPRITES;
        extern void station_init(GameData* data);
        station_init(data);
        return;

      case NEWS:
        sfx_confirm();
        read_news_terminal(data);
        return;
    }
  }

  int16_t cam_x = data->player_x;
  int16_t cam_y = data->player_y;

  if (cam_x <= 80) cam_x = 0;
  else if (cam_x - 80 > 96) cam_x = 96;
  else cam_x -= 80;

  if (cam_y <= 72) cam_y = 0;
  else if (cam_y - 72 > 112) cam_y = 112;
  else cam_y -= 72;

  scroll_x = cam_x;
  scroll_y = cam_y;
  apply_scroll();

  uint8_t sprite_x = (data->player_x - scroll_x) + 8;
  uint8_t sprite_y = (data->player_y - scroll_y) + 8;

  if (old_x != data->player_x || old_y != data->player_y) {
    move_sprite(0, sprite_x, sprite_y);
    set_sprite_tile(0, 0);
  }

  //npcs_render(scroll_x, scroll_y);
}
