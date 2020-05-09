#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GFX_FRAMEBUFFERINTERFACE_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GFX_FRAMEBUFFERINTERFACE_HPP

#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace FunnyOS::HW {

    /**
     * Configuration used to setup a FramebufferInterface
     */
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

    /**
     * Interface for drawing on a linear framebuffer.
     */
    class FramebufferInterface {
       public:
        /**
         * Constructs new FramebufferInterface
         *
         * @param config configuration used to setup this interface.
         */
        FramebufferInterface(const FramebufferConfiguration& config);

        /**
         * Puts a pixel on the screen.
         *
         * @param x X location of the pixel on the screen
         * @param y X location of the pixel on the screen
         * @param r red component (0-255)
         * @param g green component (0-255)
         * @param b blue component (0-255)
         */
        void PutPixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b);

        /**
         * @return the width of the screen in pixels.
         */
        [[nodiscard]] uint32_t GetScreenWidth() const noexcept;

        /**
         * @return the height of the screen in pixels.
         */
        [[nodiscard]] uint32_t GetScreenHeight() const noexcept;

       private:
        FramebufferConfiguration m_config;
    };

}  // namespace FunnyOS::HW

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GFX_FRAMEBUFFERINTERFACE_HPP
