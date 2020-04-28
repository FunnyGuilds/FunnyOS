#ifndef FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_BOOTDRIVEINFO_HPP
#define FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_BOOTDRIVEINFO_HPP

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::Bootparams {

    struct BootDriveInfo {
        /**
         * Number of the drive that the we are booting from.
         */
        uint8_t BootDriveNumber;

        /**
         * Number of the drive that the we are booting from.
         */
        uint8_t BootPartition;

    } F_DONT_ALIGN;

}  // namespace FunnyOS::Bootparams

#endif  // FUNNYOS_MISC_BOOTPARAMS_HEADERS_FUNNYOS_BOOTPARAMS_BOOTDRIVEINFO_HPP
