#include <FunnyOS/Kernel/GFX/ScreenManager.hpp>

namespace FunnyOS::Kernel::GFX {

    void ScreenManager::InitializeWith(HW::FramebufferConfiguration framebufferConfig, uint8_t* fonts) {
        m_framebufferInterface = Stdlib::MakeRef<HW::FramebufferInterface>(framebufferConfig);
        m_textInterface = Stdlib::MakeRef<HW::FontTerminalInterface>(fonts, m_framebufferInterface);
    }

    Stdlib::Ref<HW::FramebufferInterface>& ScreenManager::GetFramebufferInterface() {
        return m_framebufferInterface;
    }

    Stdlib::Ref<HW::FontTerminalInterface>& ScreenManager::GetTextInterface() {
        return m_textInterface;
    }

    ScreenManager::ScreenManager() = default;
}  // namespace FunnyOS::Kernel::GFX