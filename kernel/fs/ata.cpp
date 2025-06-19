#include "ata.h"
#include <stdint.h>
#include "../core/logger/logger.h"

#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_SECCOUNT0   0x1F2
#define ATA_LBA0        0x1F3
#define ATA_LBA1        0x1F4
#define ATA_LBA2        0x1F5
#define ATA_HDDEVSEL    0x1F6
#define ATA_COMMAND     0x1F7
#define ATA_STATUS      0x1F7

#define ATA_CMD_READ_PIO 0x20
#define ATA_SR_BSY     0x80
#define ATA_SR_DRQ     0x08

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void insw(uint16_t port, void* addr, int count) {
    asm volatile ("rep insw" : "+D"(addr), "+c"(count) : "d"(port) : "memory");
}

namespace ata {
    void init() {
        // No initialization needed for basic PIO
    }

    static void io_wait() {
        for (int i = 0; i < 4; ++i) inb(0x80);
    }

    bool read_sector(uint32_t lba, uint8_t* buffer) {
        logger::info("ATA: read_sector called", "ATA");
        // Wait for drive to be ready
        int spin = 0;
        while (inb(ATA_STATUS) & ATA_SR_BSY) {
            if (++spin > 1000000) {
                logger::error("ATA: Timeout waiting for BSY to clear", "ATA");
                return false;
            }
        }

        outb(ATA_HDDEVSEL, 0xE0 | ((lba >> 24) & 0x0F));
        outb(ATA_SECCOUNT0, 1);
        outb(ATA_LBA0, lba & 0xFF);
        outb(ATA_LBA1, (lba >> 8) & 0xFF);
        outb(ATA_LBA2, (lba >> 16) & 0xFF);
        outb(ATA_COMMAND, ATA_CMD_READ_PIO);

        // Wait for BSY to clear and DRQ to set
        spin = 0;
        while (true) {
            uint8_t status = inb(ATA_STATUS);
            if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;
            if (++spin > 1000000) {
                logger::error("ATA: Timeout waiting for DRQ after command", "ATA");
                return false;
            }
        }

        // Read 256 words (512 bytes)
        uint16_t* buf16 = (uint16_t*)buffer;
        for (int i = 0; i < 256; ++i) {
            uint16_t data;
            asm volatile ("inw %1, %0" : "=a"(data) : "Nd"(ATA_DATA));
            buf16[i] = data;
        }

        io_wait();
        logger::info("ATA: read_sector completed", "ATA");
        return true;
    }

    bool write_sector(uint32_t lba, const uint8_t* buffer) {
        logger::info("ATA: write_sector called", "ATA");
        int spin = 0;
        while (inb(ATA_STATUS) & ATA_SR_BSY) {
            if (++spin > 1000000) {
                logger::error("ATA: Timeout waiting for BSY to clear (write)", "ATA");
                return false;
            }
        }
        outb(ATA_HDDEVSEL, 0xE0 | ((lba >> 24) & 0x0F));
        outb(ATA_SECCOUNT0, 1);
        outb(ATA_LBA0, lba & 0xFF);
        outb(ATA_LBA1, (lba >> 8) & 0xFF);
        outb(ATA_LBA2, (lba >> 16) & 0xFF);
        outb(ATA_COMMAND, 0x30); // ATA_CMD_WRITE_PIO
        spin = 0;
        while (true) {
            uint8_t status = inb(ATA_STATUS);
            if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;
            if (++spin > 1000000) {
                return false;
            }
        }
        const uint16_t* buf16 = (const uint16_t*)buffer;
        for (int i = 0; i < 256; ++i) {
            asm volatile ("outw %0, %1" : : "a"(buf16[i]), "Nd"(ATA_DATA));
        }
        outb(ATA_COMMAND, 0xE7); // FLUSH CACHE
        io_wait();
        return true;
    }
} 