#include "file.h"
#include "fat.h"
#include <stdint.h>
#include <stddef.h>

namespace file {
    static uint8_t file_static_buffer[64 * 1024]; // 64KB static buffer
    static bool buffer_in_use = false;

    File::File() : _size(0), _pos(0), _data(nullptr), _mode(Mode::Read), _open(false) {
        for (int i = 0; i < 12; ++i) name[i] = 0;
    }
    File::~File() { close(); }

    bool File::open(const char* filename, Mode mode) {
        if (buffer_in_use) return false;
        for (int i = 0; i < 12; ++i) name[i] = filename[i];
        _pos = 0;
        _mode = mode;
        if (mode == Mode::Read) {
            int bytes = fat::read_file(filename, file_static_buffer, sizeof(file_static_buffer));
            if (bytes <= 0) return false;
            _size = bytes;
            _data = file_static_buffer;
            _open = true;
            buffer_in_use = true;
            return true;
        } else if (mode == Mode::Write) {
            _size = 0;
            _data = file_static_buffer;
            _open = true;
            buffer_in_use = true;
            return true;
        }
        return false;
    }

    int File::read(void* buffer, uint32_t size) {
        if (!_open || _mode != Mode::Read || _pos >= _size) return 0;
        uint32_t to_read = (_size - _pos > size) ? size : (_size - _pos);
        uint8_t* out = (uint8_t*)buffer;
        for (uint32_t i = 0; i < to_read; ++i) out[i] = _data[_pos + i];
        _pos += to_read;
        return to_read;
    }

    int File::write(const void* buffer, uint32_t size) {
        if (!_open || _mode != Mode::Write) return 0;
        uint32_t to_write = (size > sizeof(file_static_buffer) - _pos) ? (sizeof(file_static_buffer) - _pos) : size;
        const uint8_t* in = (const uint8_t*)buffer;
        for (uint32_t i = 0; i < to_write; ++i) _data[_pos + i] = in[i];
        _pos += to_write;
        if (_pos > _size) _size = _pos;
        return to_write;
    }

    bool File::seek(uint32_t pos) {
        if (!_open) return false;
        if (pos > _size) return false;
        _pos = pos;
        return true;
    }

    void File::close() {
        if (!_open) return;
        if (_mode == Mode::Write && _size > 0) {
            fat::create_file(name, _data, _size);
        }
        _data = nullptr;
        _size = 0;
        _pos = 0;
        _open = false;
        buffer_in_use = false;
    }

    bool File::is_open() const { return _open; }
    uint32_t File::size() const { return _size; }
    uint32_t File::tell() const { return _pos; }

    bool create(const char* filename, const uint8_t* data, uint32_t size) {
        return fat::create_file(filename, data, size);
    }
} 