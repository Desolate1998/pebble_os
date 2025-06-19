#pragma once
#include <stdint.h>

namespace ata {
    void init();
    bool read_sector(uint32_t lba, uint8_t* buffer);
    // Optionally, add write_sector if needed in the future
} 