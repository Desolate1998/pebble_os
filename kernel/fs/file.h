#pragma once
#include <stdint.h>

namespace file {
    struct File {
        char name[12];
        uint32_t size;
        uint32_t pos;
        uint8_t* data;
    };

    bool open(const char* filename, File& file);
    int read(File& file, uint8_t* buffer, uint32_t size);
    void close(File& file);
} 