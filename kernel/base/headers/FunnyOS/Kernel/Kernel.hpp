#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_KERNEL_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_KERNEL_HPP

#include <FunnyOS/Bootparams/Parameters.hpp>
#include <FunnyOS/Misc/MemoryAllocator/StaticMemoryAllocator.hpp>
#include "VMM/VirtualMemoryManager.hpp"

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
         * @return kernel's boot parameter.
         */
        [[nodiscard]] const Bootparams::Parameters& GetParameters() const;

        /**
         * @return kernel's virtual memory manager
         */
        [[nodiscard]] VMM::VirtualMemoryManager& GetVirtualMemoryManager();

        /*
         * Returns the allocator used to allocate memory in kernel heap.
         *
         * @return kernel heap allocator.
         */
        [[nodiscard]] Misc::MemoryAllocator::StaticMemoryAllocator& GetKernelAllocator();

       private:
        Kernel64();

       private:
        bool m_initialized = false;
        Bootparams::Parameters m_parameters{};
        VMM::VirtualMemoryManager m_virtualMemoryManager{};
        Misc::MemoryAllocator::StaticMemoryAllocator m_kernelAllocator{};
    };

}  // namespace FunnyOS::Kernel

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_KERNEL_HPP
