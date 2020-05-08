#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_VESA_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_VESA_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Bootparams/VESA.hpp>

namespace FunnyOS::Bootloader32 {
    using Bootparams::VbeInfoBlock;
    using Bootparams::VbeModeInfoBlock;
    using Bootparams::EdidInformation;

    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(VesaException);

    void* VesaPointerToVoidPointer(uint32_t vesaPointer);

    const VbeInfoBlock& GetVbeInfoBlock();

    const Stdlib::Optional<EdidInformation>& GetEdidInformation();

    Stdlib::Memory::SizedBuffer<VbeModeInfoBlock> GetVbeModes();

    Stdlib::Optional<uint16_t> FindVideoMode(uint32_t width, uint32_t height);

    Stdlib::Optional<uint16_t> PickBestMode();

    void SelectVideoMode(uint16_t mode);
}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_VESA_HPP
