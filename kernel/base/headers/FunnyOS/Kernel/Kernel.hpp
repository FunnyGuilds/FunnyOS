#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_KERNEL_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_KERNEL_HPP

#include <FunnyOS/Bootparams/Parameters.hpp>
#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>
#include "GFX/ScreenManager.hpp"
#include "MM/PhysicalMemoryManager.hpp"
#include "MM/VirtualMemoryManager.hpp"
#include "LogManager.hpp"

#define FK_LOGGER() FunnyOS::Kernel::Kernel64::Get().GetLogManager().GetLogger()

#define FK_LOG_INFO(message) F_LOG_INFO(FK_LOGGER(), message)
#define FK_LOG_OK(message) F_LOG_OK(FK_LOGGER(), message)
#define FK_LOG_WARNING(message) F_LOG_WARNING(FK_LOGGER(), message)
#define FK_LOG_ERROR(message) F_LOG_ERROR(FK_LOGGER(), message)
#define FK_LOG_FATAL(message) F_LOG_FATAL(FK_LOGGER(), message)
#define FK_LOG_DEBUG(message) F_LOG_DEBUG(FK_LOGGER(), message)

#define FK_LOG_INFO_F(message, ...) F_LOG_INFO_F(FK_LOGGER(), message, __VA_ARGS__)
#define FK_LOG_OK_F(message, ...) F_LOG_OK_F(FK_LOGGER(), message, __VA_ARGS__)
#define FK_LOG_WARNING_F(message, ...) F_LOG_WARNING_F(FK_LOGGER(), message, __VA_ARGS__)
#define FK_LOG_ERROR_F(message, ...) F_LOG_ERROR_F(FK_LOGGER(), message, __VA_ARGS__)
#define FK_LOG_FATAL_F(message, ...) F_LOG_FATAL_F(FK_LOGGER(), message, __VA_ARGS__)
#define FK_LOG_DEBUG_F(message, ...) F_LOG_DEBUG_F(FK_LOGGER(), message, __VA_ARGS__)

namespace FunnyOS::Kernel {

    class Kernel64 {
       public:
        /**
         * Gets the Kernel64 shared class instance.
         *
         * @return Kernel64 instance.
         */
        static Kernel64& Get();

        /**
         * Kernel main function, should never be called directly.
         *
         * @param parameters kernel boot parametesr
         */
        [[noreturn]] void Main(Bootparams::Parameters& parameters);

        /**
         * @return kernel's boot drive info.
         */
        [[nodiscard]] const Bootparams::BootDriveInfo& GetBootDriveInfo() const;

        /**
         * @return kernel's physical memory manager
         */
        [[nodiscard]] MM::PhysicalMemoryManager& GetPhysicalMemoryManager();

        /**
         * @return kernel's virtual memory manager
         */
        [[nodiscard]] MM::VirtualMemoryManager& GetVirtualMemoryManager();

        /*
         * Returns the allocator used to allocate memory in kernel heap.
         *
         * @return kernel heap allocator.
         */
        [[nodiscard]] Misc::MemoryAllocator::StaticMemoryAllocator& GetKernelAllocator();

        /**
         * Returns the log manager used by the kernel.
         *
         * @return kernel log manager
         */
        [[nodiscard]] LogManager& GetLogManager();

        /**
         * Returns the screen manager used by the kernel.
         *
         * @return kernel screen manager
         */
        [[nodiscard]] GFX::ScreenManager& GetScreenManager();

       private:
        Kernel64();

       private:
        bool m_initialized = false;
        Bootparams::BootDriveInfo m_bootDriveInfo{};
        MM::PhysicalMemoryManager m_physicalMemoryManager{};
        MM::VirtualMemoryManager m_virtualMemoryManager{m_physicalMemoryManager};
        Misc::MemoryAllocator::StaticMemoryAllocator m_kernelAllocator{};
        LogManager m_logManager{};
        GFX::ScreenManager m_screenManager{};
    };

}  // namespace FunnyOS::Kernel

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_KERNEL_HPP
