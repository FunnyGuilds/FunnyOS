#include <FunnyOS/Kernel/MM/VirtualMemoryManager.hpp>

namespace FunnyOS::Kernel::MM {
    namespace {
        inline void SetPageTableBase(void* base) {
            asm ("mov %0, %%cr3" :: "m"(base) : "memory");
        }
    }

    void VirtualMemoryManager::InitializePageTables() {

    }

    VirtualMemoryManager::VirtualMemoryManager(PhysicalMemoryManager& pmm) : m_pmm(pmm) {}

}  // namespace FunnyOS::Kernel::MM