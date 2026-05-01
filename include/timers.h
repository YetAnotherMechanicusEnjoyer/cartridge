#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

extern uint8_t timer_counters[8];
extern uint8_t timer_flags;

extern void timers_update(void);

inline void set_timer(uint8_t id, uint8_t frames) {
  id = id & 7;
  timer_counters[id] = frames;
  timer_flags &= ~(1 << id);
}

inline uint8_t check_timer(uint8_t id) {
  id = id & 7;
  uint8_t mask = 1 << id;

  if (timer_flags & mask) {
    timer_flags &= ~mask;
    return 1;
  }
  return 0;
}

inline void stop_timer(uint8_t id) {
  timer_counters[id & 7] = 0;
}

#endif /* TIMERS_H */
