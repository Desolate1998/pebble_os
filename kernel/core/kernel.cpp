#include "logger/logger.h"


extern "C" void kernel_main() {
  logger::init();
  logger::INFO("Boot successful");        // Will be ignored
  logger::WARN("Low memory");             // Will be shown
  logger::ERROR("System crash");          // Will be shown

}

