#include "Bootloader32.hpp"

// GetBootloader implementation
namespace FunnyOS::Bootloader {
    static FunnyOS::Bootloader32::Bootloader32Type bootloader32_type;

    Bootloader::BootloaderType* GetBootloader() {
        return &bootloader32_type;
    }

}  // namespace FunnyOS::Bootloader

// Entry-point
extern "C" void bootloader32_main() {
    FunnyOS::Bootloader::GetBootloader()->Main();
}