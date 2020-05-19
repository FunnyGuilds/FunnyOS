#include <FunnyOS/Kernel/GFX/ScreenManager.hpp>

namespace FunnyOS::Kernel::GFX {
    void ScreenManager::InitializeWith(Bootparams::VbeInfoBlock vbeInfo,
                                       Stdlib::Vector<Bootparams::VbeModeInfoBlock>&& videoModes,
                                       Stdlib::Optional<Bootparams::EdidInformation>&& edid,
                                       HW::FramebufferConfiguration framebufferConfig, uint8_t* fonts) {
        m_vbeInfo = vbeInfo;
        m_videoModes = videoModes;
        m_edid = edid;
        m_framebufferInterface = Stdlib::MakeRef<HW::FramebufferInterface>(framebufferConfig);
        m_textInterface = Stdlib::MakeRef<HW::FontTerminalInterface>(fonts, m_framebufferInterface);
    }

    const Bootparams::VbeInfoBlock& ScreenManager::GetVbeInfo() const {
        return m_vbeInfo;
    }

    const Stdlib::Vector<Bootparams::VbeModeInfoBlock>& ScreenManager::GetVideoModes() const {
        return m_videoModes;
    }

    const Stdlib::Optional<Bootparams::EdidInformation>& ScreenManager::GetEdid() const {
        return m_edid;
    }

    Stdlib::Ref<HW::FramebufferInterface>& ScreenManager::GetFramebufferInterface() {
        return m_framebufferInterface;
    }

    Stdlib::Ref<HW::FontTerminalInterface>& ScreenManager::GetTextInterface() {
        return m_textInterface;
    }

    ScreenManager::ScreenManager() = default;
}  // namespace FunnyOS::Kernel::GFX