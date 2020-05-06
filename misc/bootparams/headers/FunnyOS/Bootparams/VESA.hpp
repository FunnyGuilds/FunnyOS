#ifndef FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_VESA_HPP
#define FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_VESA_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>

namespace FunnyOS::Bootparams {

    namespace VbeModeAttributes {
        constexpr const uint16_t ModeSupported = 1 << 0;
        constexpr const uint16_t OptionalInformationAvailable = 1 << 1;
        constexpr const uint16_t BiosOutputSupported = 1 << 2;
        constexpr const uint16_t IsColorMode = 1 << 3;
        constexpr const uint16_t IsGraphicsMode = 1 << 4;

        // VBE v2.0+
        constexpr const uint16_t NotVgaCompatible = 1 << 5;
        constexpr const uint16_t BankSwitchedModeNotSupported = 1 << 6;
        constexpr const uint16_t LinearFramebufferModeSupported = 1 << 7;
        constexpr const uint16_t DoubleScanModeSupported = 1 << 8;

        // VBE v3.0+
        constexpr const uint16_t InterlacedMoveAvailable = 1 << 9;
        constexpr const uint16_t HardwareSupportsTriplebuffering = 1 << 10;
        constexpr const uint16_t HardwareSupportsStereoscopicDisplay = 1 << 11;
        constexpr const uint16_t DualDisplayStartAddressSupport = 1 << 12;
    }  // namespace VbeModeAttributes

    namespace VbeWindowAttributes {
        constexpr const uint16_t Exists = 1 << 0;
        constexpr const uint16_t Readable = 1 << 1;
        constexpr const uint16_t Writable = 1 << 2;
    }  // namespace VbeWindowAttributes

    enum class VbeMemoryModelType : uint8_t {
        Text = 0x00,
        CGA_Graphics = 0x01,
        HerculesGraphics = 0x02,
        Planar = 0x03,
        PackedPixelGraphics = 0x04,
        Nonchain4_256color = 0x05,
        DirectColor = 0x06,
        YUV = 0x07
    };

    struct VbeInfoBlock {
        char VbeSignature[4];
        uint16_t VbeVersion;
        uint32_t OemStringPtr;
        uint32_t Capabilities;
        uint32_t VideoModePtr;
        uint16_t TotalMemory;
        uint16_t OemSoftwareRev;
        uint32_t OemVendorNamePtr;
        uint32_t OemProductNamePtr;
        uint32_t OemProductRevPtr;
        uint8_t Reserved[222];
        uint8_t OemData[256];
    } F_DONT_ALIGN;

    struct VbeModeInfoBlock {
        uint16_t ModeAttributes;
        uint8_t WindowA_Attributes;
        uint8_t WindowB_Attributes;
        uint16_t GranularityKB;
        uint16_t SizeKB;
        uint16_t WindowA_SegmentStart;
        uint16_t WindowB_SegmentStart;
        uint32_t WindowPositioningFunctionPtr;
        uint16_t BytesPerScanline;
        uint16_t Width;
        uint16_t Height;
        uint8_t WidthOfCharacter;
        uint8_t HeightOfCharacter;
        uint8_t NumberOfMemoryPlanes;
        uint8_t BitsPerPixel;
        uint8_t NumberOfBanks;
        VbeMemoryModelType MemoryModel;
        uint8_t BankSizeKb;
        uint8_t NumberOfImagePages;
        uint8_t Reserved1;
        uint8_t RedMask;
        uint8_t RedPosition;
        uint8_t GreenMask;
        uint8_t GreenPosition;
        uint8_t BlueMask;
        uint8_t BluePosition;
        uint8_t ReservedMask;
        uint8_t ReservedPosition;
        uint8_t DirectColorModeIfo;
        uint32_t FrameBufferPhysicalAddress;
        uint32_t OffscreenMemoryOffset;
        uint16_t OffscreenMemorySize;
        uint8_t Reserved2[206];
        bool IsValid;
        uint16_t VESAVideoMode;
    } F_DONT_ALIGN;

    struct VbeInformation {
        uint32_t InfoBlockLocation;
        uint32_t ModeInfoStart;
        uint32_t ModeInfoEntries;
        uint16_t ActiveModeIndex;
    };

}  // namespace FunnyOS::Bootparams

#endif  // FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_VESA_HPP
