#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_BOOTLOADER64_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_BOOTLOADER64_HPP

#include <FunnyOS/Stdlib/System.hpp>
#include <FunnyOS/Stdlib/Logging.hpp>
#include <FunnyOS/Hardware/GDT.hpp>
#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>
#include <FunnyOS/Misc/TerminalManager/TerminalManager.hpp>

#define FB_LOGGER() FunnyOS::Bootloader64::Bootloader::Get().GetLogger()

#define FB_LOG_INFO(message)    F_LOG_INFO(FB_LOGGER(), message)
#define FB_LOG_OK(message)      F_LOG_OK(FB_LOGGER(), message)
#define FB_LOG_WARNING(message) F_LOG_WARNING(FB_LOGGER(), message)
#define FB_LOG_ERROR(message)   F_LOG_ERROR(FB_LOGGER(), message)
#define FB_LOG_FATAL(message)   F_LOG_FATAL(FB_LOGGER(), message)
#define FB_LOG_DEBUG(message)   F_LOG_DEBUG(FB_LOGGER(), message)

#define FB_LOG_INFO_F(message, ...)    F_LOG_INFO_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_OK_F(message, ...)      F_LOG_OK_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_WARNING_F(message, ...) F_LOG_WARNING_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_ERROR_F(message, ...)   F_LOG_ERROR_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_FATAL_F(message, ...)   F_LOG_FATAL_F(FB_LOGGER(), message, __VA_ARGS__)
#define FB_LOG_DEBUG_F(message, ...)   F_LOG_DEBUG_F(FB_LOGGER(), message, __VA_ARGS__)

namespace FunnyOS::Bootloader64 {

    /**
     * GDT selector for bootloader 64-bit data.
     */
    constexpr const uint16_t GDT_SELECTOR_DATA64 = 1;

    /**
     * GDT selector for bootloader 64-bit ring 0 code.
     */
    constexpr const uint16_t GDT_SELECTOR_CODE64 = 2;

    /**
     * GDT selector for bootloader 16-bit data.
     */
    constexpr const uint16_t GDT_SELECTOR_DATA16 = 3;

    /**
     * GDT selector for bootloader 16-bit code.
     */
    constexpr const uint16_t GDT_SELECTOR_CODE16 = 4;

    class Bootloader {
       public:
        /**
         * Bootloader main
         *
         * @param bootDrive id of the boot drive
         * @param bootPartition id of the boot partition
         * @param memoryTop the highest addressable bit in low memory (exclusive)
         */
        [[noreturn]] void Main(uint8_t bootDrive, uint8_t bootPartition, uintptr_t memoryTop);

        /**
         * Returns the bootloader's low memory allocator
         *
         * @return the bootloader's low memory allocator
         */
        Misc::MemoryAllocator::StaticMemoryAllocator& GetAllocator();

        /**
         * Returns the main bootloader's logger
         *
         * @return the main bootloader's logger
         */
        Stdlib::Logger& GetLogger();

        /**
         * Returns the main bootloader's terminal output
         *
         * @return the main bootloader's terminal output
         */
        Misc::TerminalManager::TerminalManager& GetTerminalManager();

       public:
        /**
         * Gets the shared bootloader instance
         *
         * @return the shared bootloader instance
         */
        static Bootloader& Get();

       private:
        Bootloader() = default;

        HW::gdt_descriptor_t m_bootloaderGDT[5];
        Misc::MemoryAllocator::StaticMemoryAllocator m_allocator{};
        Stdlib::Logger m_logger{};
        Stdlib::Ref<Misc::TerminalManager::TerminalManager> m_terminalManager{};

    };

}  // namespace FunnyOS::Bootloader64

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_BOOTLOADER64_HPP
