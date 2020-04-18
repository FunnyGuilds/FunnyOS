#include <FunnyOS/Stdlib/Memory.hpp>

#include <FunnyOS/Stdlib/Platform.hpp>

namespace FunnyOS::Stdlib::Memory {
    namespace {
        void* g_zeroMemory = reinterpret_cast<void*>(NumeralTraits::Info<uintptr_t>::MaximumValue);
    }

    void* Allocate(size_t size) {
        if (size == 0) {
            return g_zeroMemory;
        }

        return _Platform::AllocateMemoryAligned(size, 1);
    }

    void* AllocateAligned(size_t size, size_t alignment) {
        if (size == 0) {
            return g_zeroMemory;
        }

        return _Platform::AllocateMemoryAligned(size, alignment);
    }

    void* AllocateInitialized(size_t size) {
        SizedBuffer<uint8_t> memory = AllocateBuffer<uint8_t>(size);
        Memory::Set(memory, static_cast<uint8_t>(0));
        return memory.Data;
    }

    void* AllocateAlignedAndInitialized(size_t size, size_t alignment) {
        SizedBuffer<uint8_t> memory = AllocateBufferAligned<uint8_t>(size, alignment);
        Memory::Set(memory, static_cast<uint8_t>(0));
        return memory.Data;
    }

    void* Reallocate(void* data, size_t size) {
        if (data == nullptr || data == g_zeroMemory) {
            return Memory::Allocate(size);
        }

        if (size == 0) {
            Memory::Free(data);
            return g_zeroMemory;
        }

        return _Platform::ReallocateMemory(data, size);
    }

    void Free(void* data) {
        if (data == nullptr || data == g_zeroMemory) {
            return;
        }

        _Platform::FreeMemory(data);
    }
}  // namespace FunnyOS::Stdlib::Memory