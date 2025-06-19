#include "fat.h"
#include "ata.h"
#include <stdint.h>
#include <stddef.h>
#include "../core/logger/logger.h"

namespace fat {
    // BPB (BIOS Parameter Block) structure for FAT12/16
    struct __attribute__((packed)) BPB {
        uint8_t  jmp[3];
        char     oem[8];
        uint16_t bytes_per_sector;
        uint8_t  sectors_per_cluster;
        uint16_t reserved_sectors;
        uint8_t  num_fats;
        uint16_t root_entries;
        uint16_t total_sectors_short;
        uint8_t  media_descriptor;
        uint16_t sectors_per_fat;
        uint16_t sectors_per_track;
        uint16_t num_heads;
        uint32_t hidden_sectors;
        uint32_t total_sectors_long;
    };

    static BPB bpb;
    static uint32_t fat_start_lba = 0;
    static uint32_t root_dir_lba = 0;
    static uint32_t data_start_lba = 0;
    static uint16_t root_dir_entries = 0;
    static uint16_t root_dir_sectors = 0;
    static bool mounted = false;

    bool mount() {
        logger::info("FAT: Reading sector 0...", "FAT");
        uint8_t sector[512];
        if (!ata::read_sector(0, sector)) {
            logger::error("FAT: Failed to read sector 0", "FAT");
            return false;
        }
        logger::info("FAT: Sector 0 read OK", "FAT");
        bpb = *(BPB*)sector;
        if (bpb.bytes_per_sector != 512) {
            logger::error("FAT: BPB bytes_per_sector != 512", "FAT");
            return false;
        }
        fat_start_lba = bpb.reserved_sectors;
        root_dir_lba = fat_start_lba + bpb.num_fats * bpb.sectors_per_fat;
        root_dir_entries = bpb.root_entries;
        root_dir_sectors = ((root_dir_entries * 32) + 511) / 512;
        data_start_lba = root_dir_lba + root_dir_sectors;
        mounted = true;
        logger::info("FAT: mount() success", "FAT");
        return true;
    }

    // Helper: format 8.3 filename (upper, space-padded, no dot, split at last dot)
    static void format_83_name(const char* input, char* name_out, char* ext_out) {
        // Find the last dot in the filename
        int len = 0, dot_pos = -1;
        while (input[len]) {
            if (input[len] == '.') dot_pos = len;
            ++len;
        }
        // Name part: up to dot or up to 8 chars
        int i = 0, j = 0;
        while (i < len && (i < dot_pos || dot_pos == -1) && j < 8) {
            char c = input[i];
            if (c >= 'a' && c <= 'z') c -= 32;
            name_out[j++] = c;
            i++;
        }
        while (j < 8) name_out[j++] = ' ';
        name_out[8] = '\0';
        // Extension part: after dot, up to 3 chars
        j = 0;
        if (dot_pos != -1) {
            i = dot_pos + 1;
            while (input[i] && j < 3) {
                char c = input[i];
                if (c >= 'a' && c <= 'z') c -= 32;
                ext_out[j++] = c;
                i++;
            }
        }
        while (j < 3) ext_out[j++] = ' ';
        ext_out[3] = '\0';
    }

    int list_root(DirectoryEntry* entries, int max_entries) {
        if (!mounted) return 0;
        uint8_t sector[512];
        int count = 0;
        for (uint16_t s = 0; s < root_dir_sectors; ++s) {
            if (!ata::read_sector(root_dir_lba + s, sector)) break;
            for (int i = 0; i < 512; i += 32) {
                if (count >= max_entries) return count;
                uint8_t* entry = &sector[i];
                if (entry[0] == 0x00) return count; // End of entries
                if (entry[0] == 0xE5) continue; // Deleted
                if ((entry[11] & 0x08) != 0) continue; // Volume label
                DirectoryEntry& e = entries[count];
                for (int j = 0; j < 8; ++j) e.name[j] = entry[j];
                e.name[8] = '.';
                for (int j = 0; j < 3; ++j) e.name[9 + j] = entry[8 + j];
                e.name[11] = '\0';
                e.size = *(uint32_t*)&entry[28];
                e.first_cluster = (*(uint16_t*)&entry[26]);
                e.is_directory = (entry[11] & 0x10) != 0;
                ++count;
            }
        }
        return count;
    }

    // Only supports reading files in root directory, no subdirectories, no fragmentation
    int read_file(const char* filename, uint8_t* buffer, uint32_t max_size) {
        if (!mounted) return 0;
        uint8_t sector[512];
        char fat_name[9], fat_ext[4];
        format_83_name(filename, fat_name, fat_ext);
        // Find file in root directory
        for (uint16_t s = 0; s < root_dir_sectors; ++s) {
            if (!ata::read_sector(root_dir_lba + s, sector)) break;
            for (int i = 0; i < 512; i += 32) {
                uint8_t* entry = &sector[i];
                if (entry[0] == 0x00) return 0;
                if (entry[0] == 0xE5) continue;
                if ((entry[11] & 0x08) != 0) continue;
                // Compare 8.3 name
                bool match = true;
                for (int j = 0; j < 8; ++j) if (entry[j] != fat_name[j]) match = false;
                for (int j = 0; j < 3; ++j) if (entry[8 + j] != fat_ext[j]) match = false;
                if (!match) continue;
                uint32_t size = *(uint32_t*)&entry[28];
                uint16_t cluster = *(uint16_t*)&entry[26];
                uint32_t bytes_read = 0;
                while (cluster >= 2 && bytes_read < size && bytes_read < max_size) {
                    uint32_t lba = data_start_lba + (cluster - 2) * bpb.sectors_per_cluster;
                    for (uint8_t sc = 0; sc < bpb.sectors_per_cluster; ++sc) {
                        if (bytes_read >= size || bytes_read >= max_size) break;
                        if (!ata::read_sector(lba + sc, sector)) return bytes_read;
                        uint32_t to_copy = (size - bytes_read > 512) ? 512 : (size - bytes_read);
                        if (to_copy > max_size - bytes_read) to_copy = max_size - bytes_read;
                        for (uint32_t k = 0; k < to_copy; ++k) buffer[bytes_read + k] = sector[k];
                        bytes_read += to_copy;
                    }
                    // Read next cluster from FAT
                    uint32_t fat_offset = cluster * 2;
                    uint32_t fat_sector = fat_start_lba + (fat_offset / 512);
                    uint32_t fat_index = (fat_offset % 512) / 2;
                    if (!ata::read_sector(fat_sector, sector)) return bytes_read;
                    cluster = ((uint16_t*)sector)[fat_index];
                    if (cluster >= 0xFFF8) break; // End of cluster chain
                }
                return bytes_read;
            }
        }
        return 0;
    }

    // Helper: find free cluster
    static uint16_t find_free_cluster(uint8_t* fat_sector, uint32_t fat_lba) {
        for (uint16_t i = 2; i < 0xFFF0; ++i) {
            uint32_t fat_offset = i * 2;
            uint32_t fat_sector_num = fat_lba + (fat_offset / 512);
            uint32_t fat_index = (fat_offset % 512) / 2;
            uint8_t sector[512];
            if (!ata::read_sector(fat_sector_num, sector)) continue;
            uint16_t val = ((uint16_t*)sector)[fat_index];
            if (val == 0x0000) return i;
        }
        return 0;
    }

    // Helper: find free root dir entry
    static int find_free_root_entry(uint8_t* sector, uint32_t* entry_sector, int* entry_offset) {
        for (uint16_t s = 0; s < root_dir_sectors; ++s) {
            if (!ata::read_sector(root_dir_lba + s, sector)) continue;
            for (int i = 0; i < 512; i += 32) {
                if (sector[i] == 0x00 || sector[i] == 0xE5) {
                    *entry_sector = root_dir_lba + s;
                    *entry_offset = i;
                    return 1;
                }
            }
        }
        return 0;
    }

    bool create_file(const char* filename, const uint8_t* data, uint32_t size) {
        logger::info("FAT: create_file called", "FAT");
        if (!mounted) return false;
        if (size > bpb.sectors_per_cluster * 512) return false; // Only 1 cluster
        uint8_t fat_sector[512];
        // 1. Find free cluster
        uint16_t cluster = find_free_cluster(fat_sector, fat_start_lba);
        if (cluster < 2) {
            logger::error("FAT: No free cluster", "FAT");
            return false;
        }
        // 2. Mark cluster as end-of-chain in FAT
        uint32_t fat_offset = cluster * 2;
        uint32_t fat_sector_num = fat_start_lba + (fat_offset / 512);
        uint32_t fat_index = (fat_offset % 512) / 2;
        if (!ata::read_sector(fat_sector_num, fat_sector)) return false;
        ((uint16_t*)fat_sector)[fat_index] = 0xFFF8;
        if (!ata::write_sector(fat_sector_num, fat_sector)) return false;
        // 3. Write file data to cluster
        uint32_t lba = data_start_lba + (cluster - 2) * bpb.sectors_per_cluster;
        uint8_t sector[512] = {0};
        for (uint32_t i = 0; i < size && i < 512; ++i) sector[i] = data[i];
        if (!ata::write_sector(lba, sector)) return false;
        // 4. Find free root dir entry
        uint8_t dir_sector[512];
        uint32_t entry_sector;
        int entry_offset;
        if (!find_free_root_entry(dir_sector, &entry_sector, &entry_offset)) {
            logger::error("FAT: No free root dir entry", "FAT");
            return false;
        }
        // 5. Write directory entry (8.3 format)
        char fat_name[9], fat_ext[4];
        format_83_name(filename, fat_name, fat_ext);
        for (int i = 0; i < 8; ++i) dir_sector[entry_offset + i] = fat_name[i];
        for (int i = 0; i < 3; ++i) dir_sector[entry_offset + 8 + i] = fat_ext[i];
        dir_sector[entry_offset + 11] = 0x20; // attr: archive
        for (int i = 12; i < 26; ++i) dir_sector[entry_offset + i] = 0;
        *(uint16_t*)&dir_sector[entry_offset + 26] = cluster;
        *(uint32_t*)&dir_sector[entry_offset + 28] = size;
        if (!ata::write_sector(entry_sector, dir_sector)) return false;
        logger::info("FAT: create_file success", "FAT");
        return true;
    }
} 