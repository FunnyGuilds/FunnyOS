#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADER32_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADER32_HPP

#include <FunnyOS/BootloaderCommons/Bootloader.hpp>

namespace FunnyOS::Bootloader32 {

    /**
     * 32-bit implementation for BootloaderType
     */
    class Bootloader32Type : public Bootloader::BootloaderType {
       public:

        [[nodiscard]] const FunnyOS::Bootloader::BootloaderParameters& GetBootloaderParameters() override;

        [[noreturn]] void Main() override;

        [[noreturn]] F_NEVER_INLINE void Panic(const char* details) override;

        [[noreturn]] void Halt() override;
    };

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADER32_HPP