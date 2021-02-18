#include <FunnyOS/Misc/MemoryAllocator/StaticFragmentedMemoryAllocator.hpp>

#include <FunnyOS/Stdlib/Algorithm.hpp>

namespace FunnyOS::Misc::MemoryAllocator {
    void StaticFragmentedMemoryAllocator::Initialize(
        const Stdlib::Memory::SizedBuffer<MemoryFragment>& memoryFragments) {
        m_allocators.Clear();
        m_allocators.EnsureCapacity(memoryFragments.Size);

        for (const auto& fragment : memoryFragments) {
            StaticMemoryAllocator& newAllocator = m_allocators.AppendInPlace();

            newAllocator.Initialize(fragment.RegionStart, fragment.RegionEnd);
        }
    }

    void* StaticFragmentedMemoryAllocator::Allocate(size_t size, size_t alignment) noexcept {
        for (auto& allocator : m_allocators) {
            void* memory = allocator.Allocate(size, alignment);

            if (memory != nullptr) {
                return memory;
            }
        }

        return nullptr;
    }

    void StaticFragmentedMemoryAllocator::Free(void* ptr) noexcept {
        const auto address = reinterpret_cast<memoryaddress_t>(ptr);

        for (auto& allocator : m_allocators) {
            if (address <= allocator.GetMemoryStart() && address < allocator.GetMemoryEnd()) {
                allocator.Free(ptr);
                return;
            }
        }

        F_FAIL_ASSERT("no allocator for the given address");
    }

    void* StaticFragmentedMemoryAllocator::Reallocate(void* ptr, size_t size, size_t alignment) noexcept {
        const auto address = reinterpret_cast<memoryaddress_t>(ptr);

        for (auto& allocator : m_allocators) {
            if (address <= allocator.GetMemoryStart() && address < allocator.GetMemoryEnd()) {
                return this->DoReallocate(allocator, ptr, size, alignment);
            }
        }

        F_FAIL_ASSERT("no allocator for the given address");
    }

    namespace {
        template <typename Func>
        size_t GetTotal(const Stdlib::Vector<StaticMemoryAllocator>& allocators, Func func) {
            size_t total = 0;

            for (const auto& alloc : allocators) {
                total += (alloc.*func)();
            }

            return total;
        }
    }

    size_t StaticFragmentedMemoryAllocator::GetTotalFreeMemory() const noexcept {
        return GetTotal(m_allocators, &StaticMemoryAllocator::GetTotalFreeMemory);
    }

    size_t StaticFragmentedMemoryAllocator::GetAllocatedMemory() const noexcept {
        return GetTotal(m_allocators, &StaticMemoryAllocator::GetAllocatedMemory);
    }

    size_t StaticFragmentedMemoryAllocator::GetTotalAvailableMemory() const noexcept {
        return GetTotal(m_allocators, &StaticMemoryAllocator::GetTotalAvailableMemory);
    }

    const Stdlib::Vector<StaticMemoryAllocator>& StaticFragmentedMemoryAllocator::GetMemberAllocators() const {
        return m_allocators;
    }

    Stdlib::Vector<StaticMemoryAllocator>& StaticFragmentedMemoryAllocator::GetMemberAllocators() {
        return m_allocators;
    }

    void* StaticFragmentedMemoryAllocator::DoReallocate(
        StaticMemoryAllocator& alloc, void* ptr, size_t size, size_t alignment) {

        // If the new block size is equal to the old block size there is no point in reallocating
        const size_t oldBlockSize = alloc.GetMemoryBlockSize(ptr);

        if (size == oldBlockSize) {
            return ptr;
        }

        // Allocate new block
        void* newMemory = Allocate(size, alignment);
        if (newMemory == nullptr) {
            return nullptr;
        }

        // Copy data
        Stdlib::Memory::Copy(newMemory, ptr, Stdlib::Min(static_cast<size_t>(oldBlockSize), size));

        // Free old memory
        alloc.Free(ptr);
        return newMemory;
    }

}  // namespace FunnyOS::Misc::MemoryAllocator
