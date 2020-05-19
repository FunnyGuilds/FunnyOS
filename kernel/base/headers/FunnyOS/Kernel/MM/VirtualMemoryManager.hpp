#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_VIRTUALMEMORYMANAGER_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_VIRTUALMEMORYMANAGER_HPP

#include <FunnyOS/Kernel/Config.hpp>

namespace FunnyOS::Kernel {
    class Kernel64;

    namespace MM {
        class PhysicalMemoryManager;

        class VirtualMemoryManager {
           public:
            void InitializePageTables();

           private:
            VirtualMemoryManager(PhysicalMemoryManager& pmm);

            friend class ::FunnyOS::Kernel::Kernel64;

           private:
            PhysicalMemoryManager& m_pmm;
        };
    }  // namespace MM

}  // namespace FunnyOS::Kernel

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_MM_VIRTUALMEMORYMANAGER_HPP
