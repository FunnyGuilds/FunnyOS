#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMODEOPTIONS_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMODEOPTIONS_HPP

#include <FunnyOS/BootloaderCommons/Logging.hpp>
#include <FunnyOS/BootloaderCommons/Bootloader.hpp>
#include <FunnyOS/BootloaderCommons/BootloaderParameters.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>

namespace FunnyOS::Bootloader32::DebugMenu {
    struct MenuOption {
        const char* Name;
        char* (*FetchState)();
        void (*Handle)();
    };

    char* DebugModeInfo();

    void UpdateDebugMode();

    void PrintMemoryMap();

    void PrintBootloaderParameters();

    void QuitMenu();

    const MenuOption g_menuOptions[] = {{"Debug mode", &DebugModeInfo, &UpdateDebugMode},
                                        {"Print memory map", nullptr, &PrintMemoryMap},
                                        {"Print bootloader parameters", nullptr, &PrintBootloaderParameters},
                                        {"Quit menu", nullptr, &QuitMenu}};
}  // namespace FunnyOS::Bootloader32::DebugMenu

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMODEOPTIONS_HPP
