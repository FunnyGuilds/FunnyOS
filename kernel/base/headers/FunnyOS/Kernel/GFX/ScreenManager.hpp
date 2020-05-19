#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_GFX_SCREENMANAGER_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_GFX_SCREENMANAGER_HPP

#include <FunnyOS/Stdlib/Dynamic.hpp>
#include <FunnyOS/Stdlib/IntegerTypes.hpp>
#include <FunnyOS/Stdlib/Vector.hpp>
#include <FunnyOS/Hardware/GFX/FramebufferInterface.hpp>
#include <FunnyOS/Hardware/GFX/FontTerminalInterface.hpp>
#include <FunnyOS/Bootparams/VESA.hpp>

namespace FunnyOS::Kernel {
    class Kernel64;

    namespace GFX {
        class ScreenManager {
           public:
            void InitializeWith(Bootparams::VbeInfoBlock vbeInfo,
                                Stdlib::Vector<Bootparams::VbeModeInfoBlock>&& videoModes,
                                Stdlib::Optional<Bootparams::EdidInformation>&& edid,
                                HW::FramebufferConfiguration framebufferConfig, uint8_t* fonts);

            const Bootparams::VbeInfoBlock& GetVbeInfo() const;

            const Stdlib::Vector<Bootparams::VbeModeInfoBlock>& GetVideoModes() const;

            const Stdlib::Optional<Bootparams::EdidInformation>& GetEdid() const;

            Stdlib::Ref<HW::FramebufferInterface>& GetFramebufferInterface();

            Stdlib::Ref<HW::FontTerminalInterface>& GetTextInterface();

           private:
            ScreenManager();

            friend class ::FunnyOS::Kernel::Kernel64;

           private:
            Bootparams::VbeInfoBlock m_vbeInfo;
            Stdlib::Vector<Bootparams::VbeModeInfoBlock> m_videoModes;
            Stdlib::Optional<Bootparams::EdidInformation> m_edid;
            Stdlib::Ref<HW::FramebufferInterface> m_framebufferInterface{};
            Stdlib::Ref<HW::FontTerminalInterface> m_textInterface{};
        };

    }  // namespace GFX
}  // namespace FunnyOS::Kernel

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_GFX_SCREENMANAGER_HPP
