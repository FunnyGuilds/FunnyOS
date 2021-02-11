#ifndef FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_PARAMETERS_HPP
#define FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_PARAMETERS_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include "BootDriveInfo.hpp"
#include "MemoryMapDescription.hpp"
#include "VESA.hpp"

namespace FunnyOS::Bootparams {

    struct Parameters {
        /**
         * Information about booting process.
         */
        BootDriveInfo BootInfo;

        /**
         * Map of the system memory.
         */
        Stdlib::Vector<MemoryRegion> MemoryMap;

        /**
         * VESA informations.
         */
        VbeInformation Vbe;

        /**
         * Pointer to the 16x8 fonts fetched from BIOS.
         */
        uint8_t* BiosFonts;

        /**
         * Size of the BiosFonts array.
         */
        uint32_t BiosFontsSize;
    };

}  // namespace FunnyOS::Bootparams

#endif  // FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_PARAMETERS_HPP
