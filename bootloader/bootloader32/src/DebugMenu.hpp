#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMENU_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMENU_HPP

#include <FunnyOS/Hardware/PS2.hpp>

namespace FunnyOS::Bootloader32::DebugMenu {

    /**
     * Handles a scan code if debug menu is enabled.
     *
     * @param code scan code
     */
    void HandleKey(HW::PS2::ScanCode code);

    /**
     * Checks whether was the debug menu requested.
     *
     * @return  whether was the debug menu requested
     */
    [[nodiscard]] bool MenuRequested();

    /**
     * Enters the debug menu. This function will block for as long as the menu is open.
     */
    void Enter();

    /**
     * Selects active submenu.
     *
     * @param submenu active submenu index, or -1 if no submenu is active
     */
    void SelectCurrentSubmenu(int submenu);
}  // namespace FunnyOS::Bootloader32::DebugMenu

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMENU_HPP
