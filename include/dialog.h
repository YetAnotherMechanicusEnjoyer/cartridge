#ifndef DIALOG_H
#define DIALOG_H

#include <stdint.h>

#define DIALOG_MAX_WIDTH 18
#define DIALOG_MAX_LINES 2
#define DIALOG_QUEUE_SIZE 8
#define TYPE_SPEED 3

typedef enum {
  D_IDLE,
  D_TYPING,
  D_WAITING_INPUT,
  D_DONE,
} DialogState;

typedef struct {
  const char* current_text;
  uint16_t char_ptr;
  uint8_t x, y;
  uint8_t timer;
  DialogState state;
  const char* queue[DIALOG_QUEUE_SIZE];
  uint8_t q_read;
  uint8_t q_write;
} DialogManager;

void dialog_start(const char* text);
void dialog_update(void);
uint8_t dialog_is_active(void);

#endif
