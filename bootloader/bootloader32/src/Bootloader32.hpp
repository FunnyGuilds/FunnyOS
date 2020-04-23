#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADER32_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADER32_HPP

#include <FunnyOS/Bootparams/Parameters.hpp>
#include "LowMemoryAllocator.hpp"

namespace FunnyOS::Bootloader32 {

    /**
     * 32-bit implementation of BootloaderType
     */
    class Bootloader {
       public:
        /**
         * Returns the whole bootparams structure.
         *
         * @return the whole the whole bootparams structure
         */
        [[nodiscard]] Bootparams::Parameters& GetBootloaderParameters();

        /**
         * Bootloader entry point
         */
        [[noreturn]] void Main();

        /**
         * Prints the error in BIG RED TEXT with some additional debugging information and halts the execution.
         *
         * @param[in] details error details
         */
        [[noreturn]] F_NEVER_INLINE void Panic(const char* details);

        /**
         * Halts the CPU
         */
        [[noreturn]] void Halt();

        /**
         * Returns the reference for the memory allocator used by the bootloader.
         */
        [[nodiscard]] LowMemoryAllocator& GetAllocator();

        /**
         * Gets the global instance of the Bootloader class.
         *
         * @return the global instance of the Bootloader class
         */
        static Bootloader& Get();

       private:
        LowMemoryAllocator m_allocator;
    };

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADER32_HPP
