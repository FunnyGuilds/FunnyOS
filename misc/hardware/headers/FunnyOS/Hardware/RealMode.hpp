#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_REALMODE_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_REALMODE_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>

namespace FunnyOS::HW {
    using namespace Stdlib;

    union Register16 {
        struct Parts {
            uint8_t Low;
            uint8_t High;
        } F_DONT_ALIGN;

        uint16_t Value16;
        Parts Value8;
    } F_DONT_ALIGN;

    struct Registers16 {
        Register16 AX = {0};
        Register16 CX = {0};
        Register16 DX = {0};
        Register16 BX = {0};
        Register16 SI = {0};
        Register16 DI = {0};
        Register16 ES = {0};
        Register16 FS = {0};
        Register16 GS = {0};
        Register16 FLAGS = {0};
    } F_DONT_ALIGN;

    struct GDTInfo {
        uint16_t SelectorCode32;
        uint16_t SelectorData32;
        uint16_t SelectorCode16;
        uint16_t SelectorData16;
    } F_DONT_ALIGN;

    void SetupRealModeInterrupts(GDTInfo gdtInfo);

    Memory::SizedBuffer<uint8_t>& GetRealModeBuffer();

    void GetRealModeBufferAddress(uint16_t& segment, uint16_t& offset);

    void RealModeInt(uint8_t interrupt, Registers16& out);

}  // namespace FunnyOS::HW

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_REALMODE_HPP
