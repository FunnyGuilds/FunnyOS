#ifndef FUNNYOS_BOOTLOADER_COMMONS_HEADERS_FUNNYOS_BOOTLOADERCOMMONS_PORT_INPUTPORT_OUTPUT_HPP
#error "Include InputOutput.hpp instead"
#endif

#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INPUTOUTPUT_GNUC_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INPUTOUTPUT_GNUC_HPP

namespace FunnyOS::HW::InputOutput {
    /**
     * Reads and returns a byte from the port at the given address/
     */
    F_ALWAYS_INLINE inline uint8_t InputByte(uint16_t address) {
        uint8_t output;
        asm volatile("inb %%dx, %%al" : "=a"(output) : "d"(address));
        return output;
    }

    /**
     * Reads and returns a word from the port at the given address.
     */
    F_ALWAYS_INLINE inline uint16_t InputWord(uint16_t address) {
        uint16_t output;
        asm volatile("inw %%dx, %%ax" : "=a"(output) : "d"(address));
        return output;
    }

    /**
     * Reads and returns a dword from the port at the given address.
     */
    F_ALWAYS_INLINE inline uint32_t InputDword(uint16_t address) {
        uint32_t output;
        asm volatile("inl %%dx, %%eax" : "=a"(output) : "d"(address));
        return output;
    }

    /**
     * Writes a byte to the port at the given address.
     */
    F_ALWAYS_INLINE inline void OutputByte(uint16_t address, uint8_t byte) {
        asm volatile("outb %%al, %%dx" ::"a"(byte), "d"(address));
    }

    /**
     * Writes a word to the port at the given address.
     */
    F_ALWAYS_INLINE inline void OutputWord(uint16_t address, uint16_t word) {
        asm volatile("outw %%ax, %%dx" ::"a"(word), "d"(address));
    }

    /**
     * Writes a dword to the port at the given address.
     */
    F_ALWAYS_INLINE inline void OutputDword(uint16_t address, uint32_t dword) {
        asm volatile("outl %%eax, %%dx" ::"a"(dword), "d"(address));
    }

    /**
     * Waits for an IO operation to finish.
     */
    F_ALWAYS_INLINE inline void IOWait() {
        asm volatile("outb %%al, $0x80" ::"a"(static_cast<uint8_t>(0)));
    }
}  // namespace FunnyOS::HW::InputOutput

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_INPUTOUTPUT_GNUC_HPP
