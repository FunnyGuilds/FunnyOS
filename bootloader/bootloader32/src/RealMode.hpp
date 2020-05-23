#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_REALMODE_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_REALMODE_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include <FunnyOS/Stdlib/System.hpp>

namespace FunnyOS::Bootloader32 {
    struct RegisterParts {
        uint8_t Low;
        uint8_t High;
    } F_DONT_ALIGN;

    /**
     * Represents a 16-bit register.
     */
    union Register16 {
        /**
         * 16-bit value of the register.
         */
        uint16_t Value16;

        /**
         * structure holding low and high bytes of the register.
         */
        RegisterParts Value8;
    } F_DONT_ALIGN;

    union Register32 {
        /**
         * 32-bit value of the register
         */
        uint32_t Value32;

        /**
         * 16-bit value of the register.
         */
        uint16_t Value16;

        /**
         * structure holding low and high bytes of the register.
         */
        RegisterParts Value8;
    } F_DONT_ALIGN;

    /**
     * Represents all registers that can be used to make a real mode interrupt.
     */
    struct Registers32 {
        Register32 EAX    = {0};
        Register32 ECX    = {0};
        Register32 EDX    = {0};
        Register32 EBX    = {0};
        Register32 EBP    = {0};
        Register32 ESI    = {0};
        Register32 EDI    = {0};
        Register32 EFLAGS = {0};
        Register16 ES     = {0};
        Register16 FS     = {0};
        Register16 GS     = {0};
    } F_DONT_ALIGN;

    /**
     * Gets a buffer that is guaranteed to be fully located below the 1 MB mark and thus fully accessible in real mode.
     * Size of the buffer is implementation defined.
     *
     * @return real mode buffer
     */
    Stdlib::Memory::SizedBuffer<uint8_t>& GetRealModeBuffer();

    /**
     * Gets an address in a segment:offset form.
     *
     * @param[out] segment segment part of the address
     * @param[out] offset offset part of the address
     */
    void GetRealModeAddress(void* address, uint16_t& segment, uint16_t& offset);

    /**
     * Gets an address in a segment:offset form.
     *
     * @param[out] segment segment part of the address
     * @param[out] offset offset part of the address
     */
    void GetRealModeAddress(uint32_t address, uint16_t& segment, uint16_t& offset);

    /**
     * Gets the address of real mode buffer (see GetRealModeBuffer()) in a segment:offset form.
     *
     * @param[out] segment segment part of the buffer's address
     * @param[out] offset offset part of the buffer's address
     */
    void GetRealModeBufferAddress(uint16_t& segment, uint16_t& offset);

    /**
     * Executes a real mode interrupt.
     *
     * @param interrupt interrupt number.
     *
     * @param[in,out] registers value of the 16-bit registers to hold parameters and return values for the interrupt.
     */
    void RealModeInt(uint8_t interrupt, Registers32& registers);

}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_REALMODE_HPP
