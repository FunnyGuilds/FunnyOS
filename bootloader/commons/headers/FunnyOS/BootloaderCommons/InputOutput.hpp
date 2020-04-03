#ifndef FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP
#define FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

#if __GNUC__

#define PORT_INPUT(ReturnType, ARegister)                                       \
    ReturnType output;                                                          \
    __asm__ __volatile__("in " ARegister ", dx" : "=a"(output) : "d"(address)); \
    return output

#define PORT_OUTPUT(ARegister, dataParameter) \
    __asm__ __volatile__("out dx, " ARegister : : "a"(dataParameter), "d"(address))

#endif

namespace FunnyOS::Bootloader::InputOutput {

    /**
     * Reads and returns a byte fromthe port at the given addres
     */
    inline uint8_t InputByte(uint16_t address) {
        PORT_INPUT(uint8_t, "al");
    }

    /**
     * Reads and returns a word from the port at the given addres
     */
    inline uint16_t InputWord(uint16_t address) {
        PORT_INPUT(uint16_t, "ax");
    }

    /**
     * Reads and returns a dword from the port at the given address.
     */
    inline uint32_t InputDword(uint16_t address) {
        PORT_INPUT(uint32_t, "eax");
    }

    /**
     * Writes a byte to the port at the given data.
     */
    inline void OutputByte(uint16_t address, uint8_t byte) {
        PORT_OUTPUT("al", byte);
    }

    /**
     * Writes a word to the port at the given data.
     */
    inline void OutputWord(uint16_t address, uint16_t word) {
        PORT_OUTPUT("ax", word);
    }

    /**
     * Writes a dword to the port at the given data.
     */
    inline void OutputDword(uint16_t address, uint32_t dword) {
        PORT_OUTPUT("eax", dword);
    }

}  // namespace FunnyOS::Bootloader::InputOutput

#undef PORT_INPUT
#undef PORT_OUTPUT

#endif  // FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP