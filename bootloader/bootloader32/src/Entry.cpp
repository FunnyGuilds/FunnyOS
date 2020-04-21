#include "Bootloader32.hpp"

// Entry-point
extern "C" void bootloader32_main() {
    FunnyOS::Bootloader32::Bootloader::Get().Main();
}