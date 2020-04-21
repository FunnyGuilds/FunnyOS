#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMENUOPTIONS_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMENUOPTIONS_HPP

#include <FunnyOS/BootloaderCommons/Logging.hpp>
#include <FunnyOS/BootloaderCommons/Bootloader.hpp>
#include <FunnyOS/BootloaderCommons/BootloaderParameters.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>

namespace FunnyOS::Bootloader32::DebugMenu {
    using namespace Stdlib;

    class MenuOption {
       public:
        virtual void FetchName(String::StringBuffer& buffer) const = 0;

        virtual void FetchState(String::StringBuffer& buffer) const = 0;

        virtual void Enter() = 0;

        virtual void HandleKey(HW::PS2::ScanCode code) = 0;
    };

    class DebugModeOption : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;

        static DebugModeOption s_instance;
    };

    class PrintMemoryMapOption : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;

        static PrintMemoryMapOption s_instance;
    };

    class PrintBootloaderParametersOption : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;

        static PrintBootloaderParametersOption s_instance;
    };

    class PrintBootDiskParameters : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;

        static PrintBootDiskParameters s_instance;
    };

    class QuitMenuOption : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;

        static QuitMenuOption s_instance;
    };

    inline MenuOption* g_menuOptions[] = {
        &DebugModeOption::s_instance,
        &PrintMemoryMapOption::s_instance,
        &PrintBootloaderParametersOption::s_instance,
        &PrintBootDiskParameters::s_instance,
        &QuitMenuOption::s_instance,
    };
}  // namespace FunnyOS::Bootloader32::DebugMenu

#undef SIMPLE_MENU_OPTION
#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMENUOPTIONS_HPP
