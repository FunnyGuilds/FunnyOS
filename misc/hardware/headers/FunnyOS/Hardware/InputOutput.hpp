#ifndef FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP
#define FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>

#if __GNUC__

#define PORT_INPUT(ReturnType, ARegister)                               \
    ReturnType output;                                                  \
    asm volatile("in " ARegister ", dx" : "=a"(output) : "d"(address)); \
    return output

#define PORT_OUTPUT(ARegister, dataParameter) asm volatile("out dx, " ARegister : : "a"(dataParameter), "d"(address))

#endif

namespace FunnyOS::HW::InputOutput {

    /**
     * Reads and returns a byte from the port at the given address/
     */
    F_ALWAYS_INLINE inline uint8_t InputByte(uint16_t address) {
        PORT_INPUT(uint8_t, "al");
    }

    /**
     * Reads and returns a word from the port at the given address.
     */
    F_ALWAYS_INLINE inline uint16_t InputWord(uint16_t address) {
        PORT_INPUT(uint16_t, "ax");
    }

    /**
     * Reads and returns a dword from the port at the given address.
     */
    F_ALWAYS_INLINE inline uint32_t InputDword(uint16_t address) {
        PORT_INPUT(uint32_t, "eax");
    }

    /**
     * Writes a byte to the port at the given address.
     */
    F_ALWAYS_INLINE inline void OutputByte(uint16_t address, uint8_t byte) {
        PORT_OUTPUT("al", byte);
    }

    /**
     * Writes a word to the port at the given address.
     */
    F_ALWAYS_INLINE inline void OutputWord(uint16_t address, uint16_t word) {
        PORT_OUTPUT("ax", word);
    }

    /**
     * Writes a dword to the port at the given address.
     */
    F_ALWAYS_INLINE inline void OutputDword(uint16_t address, uint32_t dword) {
        PORT_OUTPUT("eax", dword);
    }

    /**
     * Waits for an IO operation to finish.
     */
    F_ALWAYS_INLINE inline void IOWait() {
#if __GNUC__
        asm volatile("out 0x80, al" ::"a"(static_cast<uint8_t>(0)));
#endif
    }
}  // namespace FunnyOS::HW::InputOutput

#undef PORT_INPUT
#undef PORT_OUTPUT

#endif  // FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP