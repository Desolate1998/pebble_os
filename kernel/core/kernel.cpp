#include "logger/logger.h"
#include "../drivers/keyboard/keyboard.h"
#include "../interrupts/idt/idt.h"
#include "../lib/std/std.h"
#include "../drivers/terminal/terminal.h"
#include "../fs/fat.h"
#include "../fs/file.h"

namespace main
{

  void kernel_initialization()
  {
    logger::init();
    logger::set_level(logger::Level::ERROR);

    logger::info("Kernel started", "KERNEL");

    logger::info("Initializing IDT...", "KERNEL");
    interrupts::init_idt();
    logger::info("IDT initialized", "KERNEL");

    logger::info("Initializing PIC...", "KERNEL");
    interrupts::init_pic();
    logger::info("PIC initialized", "KERNEL");

    logger::info("Initializing keyboard driver...", "KERNEL");
    keyboard::init();
    logger::info("Keyboard driver initialized", "KERNEL");

    logger::info("Mounting FAT file system...", "KERNEL");
    if (fat::mount()) {
      logger::info("FAT file system mounted.", "KERNEL");
    } else {
      logger::error("Failed to mount FAT file system!", "KERNEL");
    }

    asm volatile("sti");
  }

  extern "C" void kernel_main()
  {
    kernel_initialization();

    // Add a variable to track the current directory (root only for now)
    static char current_dir[64] = "/";
    while (true)
    {
      std::String command = std::get_input();
      if (command.equals("clear"))
      {
        terminal::clear();
      }
      else if (command.length() > 3 && command.c_str()[0] == 'c' && command.c_str()[1] == 'd' && command.c_str()[2] == ' ') {
        // cd <name> or cd ..
        const char* arg = command.c_str() + 3;
        while (*arg == ' ') ++arg;
        if (arg[0] == '.' && arg[1] == '.' && arg[2] == 0) {
          // Only root supported, so go to root
          current_dir[0] = '/'; current_dir[1] = 0;
          terminal::print("Changed to root directory.\n");
        } else if (arg[0] == '/' && arg[1] == 0) {
          current_dir[0] = '/'; current_dir[1] = 0;
          terminal::print("Changed to root directory.\n");
        } else {
          // No subdirectory support
          terminal::print("Subdirectories not supported.\n");
        }
      }
      else if (command.equals("ls")) {
        terminal::print("Current directory: ");
        terminal::print(current_dir);
        terminal::print("\n");
        fat::DirectoryEntry entries[32];
        int count = fat::list_root(entries, 32);
        for (int i = 0; i < count; ++i) {
          const char* raw = entries[i].name;
          char name[9], ext[4];
          int nlen = 8, elen = 3;
          for (int j = 7; j >= 0; --j) {
            if (raw[j] != ' ') { nlen = j+1; break; }
          }
          for (int j = 0; j < nlen; ++j) name[j] = raw[j];
          name[nlen] = '\0';
          for (int j = 2; j >= 0; --j) {
            if (raw[9 + j] != ' ') { elen = j+1; break; }
          }
          for (int j = 0; j < elen; ++j) ext[j] = raw[9 + j];
          ext[elen] = '\0';
          terminal::print(name);
          if (elen > 0) {
            terminal::print(".");
            terminal::print(ext);
          }
          if (entries[i].is_directory) terminal::print("/");
          terminal::print("\n");
        }
      }
      else if (command.length() > 7 && command.c_str()[0] == 'c' && command.c_str()[1] == 'r' && command.c_str()[2] == 'e' && command.c_str()[3] == 'a' && command.c_str()[4] == 't' && command.c_str()[5] == 'e' && command.c_str()[6] == ' ') {
        if (current_dir[0] != '/' || current_dir[1] != 0) {
          terminal::print("Subdirectories not supported.\n");
          continue;
        }
        // create <name> <text>
        const char* rest = command.c_str() + 7;
        int space = -1;
        for (int i = 0; rest[i]; ++i) {
          if (rest[i] == ' ') { space = i; break; }
        }
        if (space > 0) {
          char fname[32];
          int i = 0;
          for (; i < space && i < 31; ++i) fname[i] = rest[i];
          fname[i] = 0;
          const char* text = rest + space + 1;
          file::File f;
          if (f.open(fname, file::File::Mode::Write)) {
            f.write(text, std::String::strlen(text));
            f.close();
            terminal::print("File created.\n");
          } else {
            terminal::print("File create failed.\n");
          }
        } else {
          terminal::print("Usage: create <name> <text>\n");
        }
      }
      else if (command.length() > 5 && command.c_str()[0] == 'r' && command.c_str()[1] == 'e' && command.c_str()[2] == 'a' && command.c_str()[3] == 'd' && command.c_str()[4] == ' ') {
        if (current_dir[0] != '/' || current_dir[1] != 0) {
          terminal::print("Subdirectories not supported.\n");
          continue;
        }
        // read <name>
        const char* fname = command.c_str() + 5;
        file::File f;
        if (f.open(fname, file::File::Mode::Read)) {
          uint8_t buf[513];
          int n;
          while ((n = f.read(buf, 512)) > 0) {
            buf[n] = 0;
            terminal::print((const char*)buf);
          }
          f.close();
          terminal::print_char('\n');
        } else {
          terminal::print("File not found or error reading file.\n");
        }
      }
      else if (command.length() > 7 && command.c_str()[0] == 'u' && command.c_str()[1] == 'p' && command.c_str()[2] == 'd' && command.c_str()[3] == 'a' && command.c_str()[4] == 't' && command.c_str()[5] == 'e' && command.c_str()[6] == ' ') {
        if (current_dir[0] != '/' || current_dir[1] != 0) {
          terminal::print("Subdirectories not supported.\n");
          continue;
        }
        // update <name> <text>
        const char* rest = command.c_str() + 7;
        int space = -1;
        for (int i = 0; rest[i]; ++i) {
          if (rest[i] == ' ') { space = i; break; }
        }
        if (space > 0) {
          char fname[32];
          int i = 0;
          for (; i < space && i < 31; ++i) fname[i] = rest[i];
          fname[i] = 0;
          const char* text = rest + space + 1;
          file::File f;
          if (f.open(fname, file::File::Mode::Write)) {
            f.write(text, std::String::strlen(text));
            f.close();
            terminal::print("File updated.\n");
          } else {
            terminal::print("File update failed.\n");
          }
        } else {
          terminal::print("Usage: update <name> <text>\n");
        }
      }
      else if (command.length() > 7 && command.c_str()[0] == 'd' && command.c_str()[1] == 'e' && command.c_str()[2] == 'l' && command.c_str()[3] == 'e' && command.c_str()[4] == 't' && command.c_str()[5] == 'e' && command.c_str()[6] == ' ') {
        if (current_dir[0] != '/' || current_dir[1] != 0) {
          terminal::print("Subdirectories not supported.\n");
          continue;
        }
        // delete <name>
        const char* fname = command.c_str() + 7;
        // Use create with empty text to simulate delete (if no delete API)
        // If you have a real delete API, use it here
        // For now, just print not implemented
        terminal::print("Delete not implemented.\n");
      }
      else if (command.length() > 6 && command.c_str()[0] == 'm' && command.c_str()[1] == 'k' && command.c_str()[2] == 'd' && command.c_str()[3] == 'i' && command.c_str()[4] == 'r' && command.c_str()[5] == ' ') {
        // mkdir <name>
        // If your FAT implementation supports directory creation, call it here.
        // Otherwise, print not implemented.
        terminal::print("mkdir not implemented.\n");
      }
      else if (command.equals("lsraw")) {
        // Print raw directory entries for debugging
        uint8_t sector[512];
        int entry_num = 0;
        for (uint16_t s = 0; s < 16; ++s) { // up to 16 sectors (8KB root dir)
          if (!ata::read_sector(fat::root_dir_lba + s, sector)) break;
          for (int i = 0; i < 512; i += 32) {
            uint8_t* entry = &sector[i];
            terminal::print("[");
            char hex[3];
            for (int j = 0; j < 11; ++j) {
              uint8_t c = entry[j];
              if (c >= 32 && c <= 126) {
                terminal::print_char((char)c);
              } else {
                hex[0] = "0123456789ABCDEF"[(c >> 4) & 0xF];
                hex[1] = "0123456789ABCDEF"[c & 0xF];
                hex[2] = 0;
                terminal::print("<"); terminal::print(hex); terminal::print(">");
              }
            }
            terminal::print("] attr=");
            uint8_t attr = entry[11];
            hex[0] = "0123456789ABCDEF"[(attr >> 4) & 0xF];
            hex[1] = "0123456789ABCDEF"[attr & 0xF];
            hex[2] = 0;
            terminal::print(hex);
            terminal::print(" first=" );
            uint16_t cl = *(uint16_t*)&entry[26];
            // Print cluster as hex
            char clhex[5];
            clhex[0] = "0123456789ABCDEF"[(cl >> 12) & 0xF];
            clhex[1] = "0123456789ABCDEF"[(cl >> 8) & 0xF];
            clhex[2] = "0123456789ABCDEF"[(cl >> 4) & 0xF];
            clhex[3] = "0123456789ABCDEF"[cl & 0xF];
            clhex[4] = 0;
            terminal::print(clhex);
            terminal::print(" size=");
            uint32_t sz = *(uint32_t*)&entry[28];
            // Print size as decimal
            char szbuf[12];
            int szlen = 0;
            if (sz == 0) { szbuf[szlen++] = '0'; }
            else {
              uint32_t tmp = sz;
              char rev[12]; int r = 0;
              while (tmp > 0) { rev[r++] = '0' + (tmp % 10); tmp /= 10; }
              while (r > 0) szbuf[szlen++] = rev[--r];
            }
            szbuf[szlen] = 0;
            terminal::print(szbuf);
            terminal::print(" raw0x");
            // Print first byte as hex
            char b0[3];
            b0[0] = "0123456789ABCDEF"[(entry[0] >> 4) & 0xF];
            b0[1] = "0123456789ABCDEF"[entry[0] & 0xF];
            b0[2] = 0;
            terminal::print(b0);
            terminal::print("\n");
            ++entry_num;
          }
        }
      }
      else {
        terminal::print("Unknown command.\n");
      }
    }

    while (true)
    {
      asm volatile("hlt");
    }
  }
}
