#include "dodge_save.h"
#include "sram.h"

#include <stdint.h>

void dodge_save_high_score(GameData* data)
{
    sram_write(sizeof(SaveData), (uint8_t*)&data->current_save, sizeof(SaveData));
}
