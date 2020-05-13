#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_GFX_SCREENMANAGER_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_GFX_SCREENMANAGER_HPP

#include <FunnyOS/Stdlib/Dynamic.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Hardware/GFX/FramebufferInterface.hpp>
#include <FunnyOS/Hardware/GFX/FontTerminalInterface.hpp>

namespace FunnyOS::Kernel {
    class Kernel64;

    namespace GFX {
        class ScreenManager {
           public:
            void InitializeWith(HW::FramebufferConfiguration framebufferConfig, uint8_t* fonts);

            Stdlib::Ref<HW::FramebufferInterface>& GetFramebufferInterface();

            Stdlib::Ref<HW::FontTerminalInterface>& GetTextInterface();

           private:
            ScreenManager();

            friend class ::FunnyOS::Kernel::Kernel64;

           private:
            Stdlib::Ref<HW::FramebufferInterface> m_framebufferInterface{};
            Stdlib::Ref<HW::FontTerminalInterface> m_textInterface{};
        };

    }  // namespace GFX
}  // namespace FunnyOS::Kernel

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_GFX_SCREENMANAGER_HPP
