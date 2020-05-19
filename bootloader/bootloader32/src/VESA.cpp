#include "VESA.hpp"

#include "RealMode.hpp"

namespace FunnyOS::Bootloader32 {
    namespace {
        constexpr const uint16_t INVALID_MODE = Stdlib::NumeralTraits::Info<uint16_t>::MaximumValue;
        constexpr const uint8_t VBE_FUNCTION_SUPPORTED = 0x4F;

        bool g_ignoreEdid = false;

        struct DefaultResolution {
            uint32_t Width;
            uint32_t Height;
        };

        constexpr const DefaultResolution g_defaultResolutions[] = {
            {720, 480}, {640, 480}, {360, 480}, {320, 480}, {720, 240}, {640, 240}, {360, 240}, {320, 240},
        };

        F_SECTION(".real.data") VbeInfoBlock g_vbeInfoBlock;
        bool g_vbeInfoBlockInitialized = false;

        void FetchVbeInfoBlock() {
            uint16_t bufferSegment;
            uint16_t bufferOffset;
            GetRealModeAddress(&g_vbeInfoBlock, bufferSegment, bufferOffset);

            Registers32 registers16;
            registers16.EAX.Value16 = 0x4F00;
            registers16.ES.Value16 = bufferSegment;
            registers16.EDI.Value16 = bufferOffset;

            g_vbeInfoBlock.VbeSignature[0] = 'V';
            g_vbeInfoBlock.VbeSignature[1] = 'B';
            g_vbeInfoBlock.VbeSignature[2] = 'E';
            g_vbeInfoBlock.VbeSignature[3] = '2';
            RealModeInt(0x10, registers16);

            if (registers16.EAX.Value8.Low != VBE_FUNCTION_SUPPORTED) {
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

                Registers32 registers16;
                registers16.EAX.Value16 = 0x4F01;
                registers16.ECX.Value16 = modes[i];
                registers16.ES.Value16 = bufferSegment;
                registers16.EDI.Value16 = bufferOffset;

                RealModeInt(0x10, registers16);
                if (registers16.EAX.Value8.Low != VBE_FUNCTION_SUPPORTED || registers16.EAX.Value8.High != 0x00) {
                    buffer[i]->IsValid = false;
                } else {
                    Stdlib::Memory::Copy(buffer[i], GetRealModeBuffer().Data, sizeof(VbeModeInfoBlock));
                    buffer[i]->IsValid = true;
                }

                buffer[i]->VESAVideoMode = modes[i];
            }

            return buffer;
        }

        Stdlib::Optional<EdidInformation> FetchEdidInformation() {
            uint16_t bufferSegment;
            uint16_t bufferOffset;
            GetRealModeBufferAddress(bufferSegment, bufferOffset);

            Registers32 registers16;
            registers16.EAX.Value16 = 0x4F15;
            registers16.EBX.Value16 = 0x001;
            registers16.ECX.Value16 = 0;
            registers16.EDX.Value16 = 0;
            registers16.ES.Value16 = bufferSegment;
            registers16.EDI.Value16 = bufferOffset;
            RealModeInt(0x10, registers16);

            if (registers16.EAX.Value8.Low != VBE_FUNCTION_SUPPORTED || registers16.EAX.Value8.High != 0) {
                return Stdlib::EmptyOptional<EdidInformation>();
            }

            return Stdlib::MakeOptional<EdidInformation>(*reinterpret_cast<EdidInformation*>(GetRealModeBuffer().Data));
        }
    }  // namespace

    void* VesaPointerToVoidPointer(uint32_t vesaPointer) {
        auto* ptr = reinterpret_cast<uint8_t*>((vesaPointer & 0xFFFF0000) >> 12);
        ptr += vesaPointer & 0xFFFF;
        return ptr;
    }

    VbeInfoBlock& GetVbeInfoBlock() {
        if (!g_vbeInfoBlockInitialized) {
            FetchVbeInfoBlock();
            g_vbeInfoBlockInitialized = true;
        }

        return g_vbeInfoBlock;
    }

    Stdlib::Optional<EdidInformation>& GetEdidInformation() {
        static Stdlib::Optional<EdidInformation> c_edid = FetchEdidInformation();
        return c_edid;
    }

    Stdlib::Memory::SizedBuffer<VbeModeInfoBlock> GetVbeModes() {
        static Stdlib::Memory::SizedBuffer<VbeModeInfoBlock> c_modes = FetchVbeModes();
        return c_modes;
    }

    Stdlib::Optional<uint16_t> FindVideoMode(uint32_t width, uint32_t height) {
        using namespace Bootparams;
        constexpr const uint16_t requiredAttributes =
            VbeModeAttributes::IsGraphicsMode | VbeModeAttributes::LinearFramebufferModeSupported;

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

            if (mode.BitsPerPixel != 32 || mode.RedPosition % 8 != 0 || mode.GreenPosition % 8 != 0 ||
                mode.BluePosition % 8 != 0) {
                continue;
            }

            if (mode.Width != width || mode.Height != height) {
                continue;
            }

            return Stdlib::MakeOptional<uint16_t>(i);
        }

        return Stdlib::EmptyOptional<uint16_t>();
    }

    Stdlib::Optional<uint16_t> PickBestMode() {
        const auto& edid = GetEdidInformation();

        if (g_ignoreEdid) {
            uint16_t bestMode = INVALID_MODE;
            uint32_t bestWidth = 0;
            uint32_t bestHeight = 0;

            for (const auto& mode : GetVbeModes()) {
                if (mode.Width * mode.Height <= bestWidth * bestHeight) {
                    continue;
                }

                auto supportedMode = FindVideoMode(mode.Width, mode.Height);
                if (!supportedMode) {
                    // mode is not supported
                    continue;
                }

                bestWidth = mode.Width;
                bestHeight = mode.Height;
                bestMode = supportedMode.GetValue();
            }

            if (bestMode != INVALID_MODE) {
                return Stdlib::MakeOptional<uint16_t>(bestMode);
            }
        }

        if (edid) {
            uint16_t edidBestMode = INVALID_MODE;
            uint32_t edidWidth = 0;
            uint32_t edidHeight = 0;

            // Find best edid resolution
            for (int i = 0; i < 3; i++) {
                uint32_t currentWidth = 0;
                uint32_t currentHeight = 0;
                edid->FetchMaxResolution(i, currentWidth, currentHeight);

                if (currentWidth == 0 || currentHeight == 0) {
                    continue;
                }
                Stdlib::Optional<uint16_t> mode = FindVideoMode(currentWidth, currentHeight);

                if (!mode) {
                    continue;
                }

                if (currentWidth * currentHeight > edidWidth * edidHeight) {
                    edidWidth = currentWidth;
                    edidHeight = currentHeight;
                    edidBestMode = mode.GetValue();
                }
            }

            if (edidBestMode != INVALID_MODE) {
                return Stdlib::MakeOptional<uint16_t>(edidBestMode);
            }
        }

        // No edid, use defaults
        for (size_t i = 0; i < sizeof(g_defaultResolutions); i++) {
            auto mode = FindVideoMode(g_defaultResolutions[i].Width, g_defaultResolutions[i].Height);
            if (mode) {
                return mode;
            }
        }

        return Stdlib::EmptyOptional<uint16_t>();
    }

    void SelectVideoMode(uint16_t mode) {
        Registers32 registers16;
        registers16.EAX.Value16 = 0x4F02;
        registers16.EBX.Value16 = GetVbeModes()[mode]->VESAVideoMode;

        RealModeInt(0x10, registers16);
    }

    bool IgnoreEdid() {
        return g_ignoreEdid;
    }

    void SetIgnoreEdid(bool ignore) {
        g_ignoreEdid = ignore;
    }
}  // namespace FunnyOS::Bootloader32