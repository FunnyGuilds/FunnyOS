#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADER32_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADER32_HPP

#include <FunnyOS/Bootparams/Parameters.hpp>
#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>

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
        [[nodiscard]] Misc::MemoryAllocator::StaticMemoryAllocator& GetAllocator();

        /**
         * Returns whether or not the bootloader should pause and wait for use input before jumping to kernel.
         *
         * @return  whether or not the bootloader should pause and wait for use input before jumping to kernel
         */
        bool IsPauseBeforeBoot() const;

        /**
         * Sets whether or not the bootloader should pause and wait for use input before jumping to kernel.
         *
         * @param pauseBeforeBoot whether or not to pause
         */
        void SetPauseBeforeBoot(bool pauseBeforeBoot);

       public:
        /**
         * Gets the global instance of the Bootloader class.
         *
         * @return the global instance of the Bootloader class
         */
        static Bootloader& Get();

       private:
        Misc::MemoryAllocator::StaticMemoryAllocator m_allocator;
        bool m_pauseBeforeBoot = false;
    };

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_BOOTLOADER32_HPP
