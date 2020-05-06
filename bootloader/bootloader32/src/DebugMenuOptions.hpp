#ifndef FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMENUOPTIONS_HPP
#define FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMENUOPTIONS_HPP
#include <FunnyOS/Bootparams/Parameters.hpp>
#include <FunnyOS/Hardware/PS2.hpp>
#include <FunnyOS/Stdlib/Memory.hpp>
#include "Bootloader32.hpp"
#include "Logging.hpp"

namespace FunnyOS::Bootloader32::DebugMenu {
    using namespace Stdlib;

    class MenuOption {
       public:
        virtual void FetchName(String::StringBuffer& buffer) const = 0;

        virtual void FetchState(String::StringBuffer& buffer) const = 0;

        virtual void Enter() = 0;

        virtual void HandleKey(HW::PS2::ScanCode code) = 0;
    };

    class SimpleSwitchModeOption : public MenuOption {
       public:
        virtual void SetMode(bool mode) = 0;

        [[nodiscard]] virtual bool GetMode() const = 0;

       public:
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;
    };

    class DebugModeOption : public SimpleSwitchModeOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void SetMode(bool mode) override;
        [[nodiscard]] bool GetMode() const override;
    };

    class LogToSerial : public SimpleSwitchModeOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void SetMode(bool mode) override;
        [[nodiscard]] bool GetMode() const override;
    };

    class DebugDiskIOOption : public SimpleSwitchModeOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void SetMode(bool mode) override;
        [[nodiscard]] bool GetMode() const override;
    };

    class DebugElfLoaderOption : public SimpleSwitchModeOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void SetMode(bool mode) override;
        [[nodiscard]] bool GetMode() const override;
    };

    class PauseBeforeBootOption : public SimpleSwitchModeOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void SetMode(bool mode) override;
        [[nodiscard]] bool GetMode() const override;
    };

    class PrintMemoryMapOption : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;
    };

    class PrintBootloaderParametersOption : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;
    };

    class PrintBootDiskParameters : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;
    };

    class CPUIDInfo : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;
    };

    class VESAInfo : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;

       private:
        void PrintVideoMode() const;

       private:
        size_t m_currentVideoMode = 0;
    };

    class QuitMenuOption : public MenuOption {
       public:
        void FetchName(String::StringBuffer& buffer) const override;
        void FetchState(String::StringBuffer& buffer) const override;
        void Enter() override;
        void HandleKey(HW::PS2::ScanCode code) override;
    };

    Memory::SizedBuffer<MenuOption*> GetMenuOptions();
}  // namespace FunnyOS::Bootloader32::DebugMenu

#endif  // FUNNYOS_BOOTLOADER_BOOTLOADER32_SRC_DEBUGMENUOPTIONS_HPP
