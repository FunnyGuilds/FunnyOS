#ifndef FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP
#define FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::HW::InputOutput {

    /**
     * Reads and returns a byte from the port at the given address/
     */
    inline uint8_t InputByte(uint16_t address);

    /**
     * Reads and returns a word from the port at the given address.
     */
    inline uint16_t InputWord(uint16_t address);

    /**
     * Reads and returns a dword from the port at the given address.
     */
    inline uint32_t InputDword(uint16_t address);

    /**
     * Writes a byte to the port at the given address.
     */
    inline void OutputByte(uint16_t address, uint8_t byte);

    /**
     * Writes a word to the port at the given address.
     */
    inline void OutputWord(uint16_t address, uint16_t word);

    /**
     * Writes a dword to the port at the given address.
     */
    inline void OutputDword(uint16_t address, uint32_t dword);

    /**
     * Waits for an IO operation to finish.
     */
    inline void IOWait();

}  // namespace FunnyOS::HW::InputOutput

#ifdef __GNUC__
#include "InputOutput_GNUC.tcc"
#endif
#endif  // FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP