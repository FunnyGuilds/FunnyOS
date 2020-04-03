#ifndef FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_BOOTLOADER_HPP
#define FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_BOOTLOADER_HPP

#include "BootloaderParameters.hpp"
#include "LowMemoryAllocator.hpp"

namespace FunnyOS::Bootloader {

    /**
     * Base class for 32-bit and 64-bit bootloader.
     */
    class BootloaderType {
       public:
        /**
         * Bootloader entry point
         *
         * @param[in] args arguments collected by the pre-bootloader asm code
         */
        virtual void Main(const BootloaderParameters& args);

        /**
         * Prints the error in BIG RED TEXT with some additional debugging information and halts the execution.
         * @param[in] details error details
         */
        [[noreturn]] virtual FOS_NEVER_INLINE void Panic(const char* details) = 0;

        /**
         * Halts the CPU
         */
        [[noreturn]] virtual void Halt() = 0;

        /**
         * Returns the reference for the memory allocator used by the bootloader.
         */
        [[nodiscard]] LowMemoryAllocator& GetAllocator();

       private:
        LowMemoryAllocator m_allocator{};
    };

    /**
     * Gets the BootloaderType instance.
     */
    extern BootloaderType* GetBootloader();

}  // namespace FunnyOS::Bootloader

#endif  // FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_BOOTLOADER_HPP
