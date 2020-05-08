#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GFX_FRAMEBUFFERINTERFACE_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GFX_FRAMEBUFFERINTERFACE_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::HW {

    struct FramebufferConfiguration {
        void* Location;
        uint32_t ScreenWidth;
        uint32_t ScreenHeight;
        uint32_t BPS;
        uint32_t BPP;
        uint8_t RedPosition;
        uint8_t GreenPosition;
        uint8_t BluePosition;
    };

    class FramebufferInterface {
       public:
        FramebufferInterface(const FramebufferConfiguration& config);

        void PutPixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b);

        [[nodiscard]] uint32_t GetScreenWidth() const noexcept;

        [[nodiscard]] uint32_t GetScreenHeight() const noexcept;

       private:
        FramebufferConfiguration m_config;
    };

}  // namespace FunnyOS::HW

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GFX_FRAMEBUFFERINTERFACE_HPP
