#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_VESA_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_VESA_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Bootparams/VESA.hpp>

namespace FunnyOS::Bootloader32 {
    using Bootparams::EdidInformation;
    using Bootparams::VbeInfoBlock;
    using Bootparams::VbeModeInfoBlock;

    /**
     * Converts a real mode pointer to a void*.
     *
     * @param vesaPointer real mode pointer (segment:offset)
     * @return a void* pointing to the same location as the real mode pointer.
     */
    void* VesaPointerToVoidPointer(uint32_t vesaPointer);

    /**
     * Returns the VbeInfoBlock fetched from BIOS.
     *
     * @return the VbeInfoBlock
     */
    VbeInfoBlock& GetVbeInfoBlock();

    /**
     * Returns the Extended Display Identification Data fetched from BIOS.
     *
     * @return the EdidInformation or empty optional if EDID not supported.
     */
    Stdlib::Optional<EdidInformation>& GetEdidInformation();

    /**
     * Gets a buffer that contains all available video modes reported by the graphics card.
     *
     * @return all available video modes reported by the graphics card.
     */
    Stdlib::Memory::SizedBuffer<VbeModeInfoBlock> GetVbeModes();

    /**
     * Finds a supported video video mode with the given width and height.
     *
     * @param width width
     * @param height height
     * @return the video mode ID or empty optional if none found.
     */
    Stdlib::Optional<uint16_t> FindVideoMode(uint32_t width, uint32_t height);

    /**
     * Tries to find a best video mode or fall backs to the default VGA modes if it cannot determine the best one.
     *
     * @return the video mode ID or empty optional if the video card does not support anything.
     */
    Stdlib::Optional<uint16_t> PickBestMode();

    /**
     * Sets current video mode to the mode with the given ID.
     *
     * @param mode mode ID to set
     */
    void SelectVideoMode(uint16_t mode);
}  // namespace FunnyOS::Bootloader32

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_VESA_HPP
