#pragma once
#include <stdint.h>

namespace file {
    class File {
    public:
        enum class Mode { Read, Write };
        File();
        ~File();
        bool open(const char* filename, Mode mode);
        int read(void* buffer, uint32_t size);
        int write(const void* buffer, uint32_t size);
        bool seek(uint32_t pos);
        void close();
        bool is_open() const;
        uint32_t size() const;
        uint32_t tell() const;
    private:
        char name[12];
        uint32_t _size;
        uint32_t _pos;
        uint8_t* _data;
        Mode _mode;
        bool _open;
    };
} 