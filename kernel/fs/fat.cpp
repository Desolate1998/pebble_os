#include "fat.h"
#include "ata.h"
#include <stdint.h>

namespace fat {
    bool mount() {
        // TODO: Implement FAT mount (read BPB, etc.)
        return false;
    }

    int list_root(DirectoryEntry* entries, int max_entries) {
        // TODO: Implement root directory listing
        return 0;
    }

    int read_file(const char* filename, uint8_t* buffer, uint32_t max_size) {
        // TODO: Implement file reading
        return 0;
    }
} 