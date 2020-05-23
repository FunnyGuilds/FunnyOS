#ifndef FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_POINTER32_HPP
#define FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_POINTER32_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/System.hpp>

namespace FunnyOS::Bootparams {

    template <typename T>
    class Pointer32 {
       public:
        constexpr inline Pointer32() : m_value(0) {}

        constexpr inline Pointer32(uint32_t ptr) : m_value(ptr) {}

        inline Pointer32(T* ptr) : m_value(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(ptr))) {
            F_ASSERT(
                reinterpret_cast<uint64_t>(ptr) <= Stdlib::NumeralTraits::Info<uint32_t>::MaximumValue,
                "ptr not accessible");
        }

        constexpr inline operator uint32_t() const {
            return m_value;
        }

        inline operator T*() const {
            return reinterpret_cast<T*>(m_value);
        }

        inline T& operator[](size_t i) const {
            return operator T*()[i];
        }

        inline T& operator*() const {
            return *(operator T*());
        }

        inline T* operator->() const {
            return operator T*();
        }

       private:
        uint32_t m_value;
    };

}  // namespace FunnyOS::Bootparams

#endif  // FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_POINTER32_HPP
