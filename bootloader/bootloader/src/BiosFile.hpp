#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_BIOSFILE_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_BIOSFILE_HPP

#include <FunnyOS/Stdlib/File.hpp>
#include <FunnyOS/Bootparams/BootDriveInfo.hpp>

namespace FunnyOS::Bootloader64 {

    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(FileSystemException);

    void InitializeFileSystem(Bootparams::BootDriveInfo& drive);

    Stdlib::Optional<Stdlib::File> OpenFile(const char* fileName);

} // namespace FunnyOS::Bootloader64

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER_SRC_BIOSFILE_HPP
