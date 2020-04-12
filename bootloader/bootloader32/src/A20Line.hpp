#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_A20LINE_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_A20LINE_HPP

namespace FunnyOS::Bootloader32::A20 {

    /**
     * Checks whether or not the A20 line is enabled.
     * @return if A20 line is enabled
     */
    bool IsEnabled();

    /**
     * Tries to enable the A20 gate via the fast A20 gate port.
     */
    void EnableA20WithFastA20();

    /**
     * Tries to enable the A20 gate via the standard method using the keyboard controller.
     */
    void EnableA20Traditionally();

    /**
     * Tries to enable the A20 line using all available methods.
     */
    void TryEnable();
}  // namespace FunnyOS::Bootloader32::A20

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_A20LINE_HPP
