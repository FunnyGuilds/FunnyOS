#include "VESA.hpp"

#include "RealMode.hpp"

namespace FunnyOS::Bootloader32 {
    namespace {
        constexpr const uint8_t VBE_FUNCTION_SUPPORTED = 0x4F;

        F_SECTION(".real.data") VbeInfoBlock g_vbeInfoBlock;
        bool g_vbeInfoBlockInitialized = false;

        void FetchVbeInfoBlock() {
            uint16_t bufferSegment;
            uint16_t bufferOffset;
            GetRealModeAddress(&g_vbeInfoBlock, bufferSegment, bufferOffset);

            Registers16 registers16;
            registers16.AX.Value16 = 0x4F00;
            registers16.ES.Value16 = bufferSegment;
            registers16.DI.Value16 = bufferOffset;

            g_vbeInfoBlock.VbeSignature[0] = 'V';
            g_vbeInfoBlock.VbeSignature[1] = 'B';
            g_vbeInfoBlock.VbeSignature[2] = 'E';
            g_vbeInfoBlock.VbeSignature[3] = '2';
            RealModeInt(0x10, registers16);

            if (registers16.AX.Value8.Low != VBE_FUNCTION_SUPPORTED) {
                g_vbeInfoBlock.VbeVersion = 0;
            }
        }

        Stdlib::Memory::SizedBuffer<VbeModeInfoBlock> FetchVbeModes() {
            auto* modes = static_cast<uint16_t*>(VesaPointerToVoidPointer(GetVbeInfoBlock().VideoModePtr));

            size_t modesCount = 0;
            while (modes[modesCount] != 0xFFFF) {
                modesCount++;
            }

            auto buffer = Stdlib::Memory::AllocateBuffer<VbeModeInfoBlock>(modesCount);
            uint16_t bufferSegment;
            uint16_t bufferOffset;
            GetRealModeBufferAddress(bufferSegment, bufferOffset);

            for (size_t i = 0; i < modesCount; i++) {
                Stdlib::Memory::Set<uint8_t>(GetRealModeBuffer(), 0);

                Registers16 registers16;
                registers16.AX.Value16 = 0x4F01;
                registers16.CX.Value16 = modes[i];
                registers16.ES.Value16 = bufferSegment;
                registers16.DI.Value16 = bufferOffset;

                RealModeInt(0x10, registers16);
                if (registers16.AX.Value8.Low != VBE_FUNCTION_SUPPORTED || registers16.AX.Value8.High != 0x00) {
                    buffer[i]->IsValid = false;
                } else {
                    Stdlib::Memory::Copy(buffer[i], GetRealModeBuffer().Data, sizeof(VbeModeInfoBlock));
                    buffer[i]->IsValid = true;
                }

                buffer[i]->VESAVideoMode = modes[i];
            }

            return buffer;
        }
    }  // namespace

    void* VesaPointerToVoidPointer(uint32_t vesaPointer) {
        auto* ptr = reinterpret_cast<uint8_t*>((vesaPointer & 0xFFFF0000) >> 12);
        ptr += vesaPointer & 0xFFFF;
        return ptr;
    }

    const VbeInfoBlock& GetVbeInfoBlock() {
        if (!g_vbeInfoBlockInitialized) {
            FetchVbeInfoBlock();
            g_vbeInfoBlockInitialized = true;
        }

        return g_vbeInfoBlock;
    }

    Stdlib::Memory::SizedBuffer<VbeModeInfoBlock> GetVbeModes() {
        static Stdlib::Memory::SizedBuffer<VbeModeInfoBlock> c_modes = FetchVbeModes();
        return c_modes;
    }

    Stdlib::Optional<uint16_t> PickBestMode() { // TODO: USE EDID
        using namespace Bootparams;
        constexpr const uint16_t INVALID_MODE = Stdlib::NumeralTraits::Info<uint16_t>::MaximumValue;
        constexpr const uint16_t requiredAttributes =
            VbeModeAttributes::IsGraphicsMode | VbeModeAttributes::LinearFramebufferModeSupported;

        uint16_t bestMode = INVALID_MODE;

        for (size_t i = 0; i < GetVbeModes().Size; i++) {
            const auto& mode = *GetVbeModes()[i];

            if (!mode.IsValid || (mode.ModeAttributes & VbeModeAttributes::ModeSupported) == 0) {
                continue;
            }

            if ((mode.ModeAttributes & requiredAttributes) != requiredAttributes) {
                continue;
            }

            if (mode.MemoryModel != VbeMemoryModelType::DirectColor &&
                mode.MemoryModel != VbeMemoryModelType::PackedPixelGraphics) {
                continue;
            }

            if (mode.BitsPerPixel != 32) {
                // TODO: Support any value divisible by 8
                continue;
            }

            if (bestMode == INVALID_MODE) {
                bestMode = i;
                continue;
            }

            const auto& best = *GetVbeModes()[bestMode];
            const uint32_t bestResolution = static_cast<uint32_t>(best.Width) * static_cast<uint32_t>(best.Height);
            const uint32_t currentResolution = static_cast<uint32_t>(mode.Width) * static_cast<uint32_t>(mode.Height);

            if (bestResolution > currentResolution) {
                continue;
            }

            if (best.BitsPerPixel > mode.BitsPerPixel) {
                continue;
            }

            bestMode = i;
        }

        return bestMode == INVALID_MODE ? Stdlib::EmptyOptional<uint16_t>() : Stdlib::MakeOptional<uint16_t>(bestMode);
    }

    void SelectVideoMode(uint16_t mode) {
        Registers16 registers16;
        registers16.AX.Value16 = 0x4F02;
        registers16.BX.Value16 = GetVbeModes()[mode]->VESAVideoMode;

        RealModeInt(0x10, registers16);
    }
}  // namespace FunnyOS::Bootloader32