#pragma once
#include <stdint.h>

namespace fat {
    struct DirectoryEntry {
        char name[12]; // 8.3 format + null terminator
        uint32_t size;
        uint32_t first_cluster;
        bool is_directory;
    };

    bool mount();
    int list_root(DirectoryEntry* entries, int max_entries);
    int read_file(const char* filename, uint8_t* buffer, uint32_t max_size);
} 