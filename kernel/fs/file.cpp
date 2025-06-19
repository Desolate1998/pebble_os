#include "file.h"
#include "fat.h"
#include <stdint.h>

namespace file {
    bool open(const char* filename, File& file) {
        // TODO: Use fat::read_file to load file into memory
        return false;
    }

    int read(File& file, uint8_t* buffer, uint32_t size) {
        // TODO: Implement file read (from memory buffer)
        return 0;
    }

    void close(File& file) {
        // TODO: Implement file close (free memory, etc.)
    }
} 