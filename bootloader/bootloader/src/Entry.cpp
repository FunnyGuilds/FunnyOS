#include "Bootloader.hpp"

extern "C" {

static void bootloader64_call_real_main() {
    for (;;) {
    }
}

// Entry-point
[[noreturn]] F_SECTION(".intro") void bootloader64_main(uint8_t bootDrive, uint8_t bootPartition, uintptr_t memoryTop) {
    FunnyOS::Bootloader64::Bootloader::Get().Main(bootPartition, bootPartition, memoryTop);
}

}