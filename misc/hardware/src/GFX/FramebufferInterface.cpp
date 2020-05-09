#include <FunnyOS/Hardware/GFX/FramebufferInterface.hpp>

namespace FunnyOS::HW {

    namespace {
        void PutPixelColor(uint8_t* ptrBase, uint8_t position, uint8_t value) {
            *(ptrBase + (position / 8)) = value;
        }
    }  // namespace

    FramebufferInterface::FramebufferInterface(const FramebufferConfiguration& config) : m_config(config) {}

    // NOLINTNEXTLINE(readability-make-member-function-const)
    void FramebufferInterface::PutPixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b) {
        auto* ptr = reinterpret_cast<uint8_t*>(m_config.Location) + m_config.BPS * y + m_config.BPP * x;
        PutPixelColor(ptr, m_config.RedPosition, r);
        PutPixelColor(ptr, m_config.GreenPosition, g);
        PutPixelColor(ptr, m_config.BluePosition, b);
    }

    uint32_t FramebufferInterface::GetScreenWidth() const noexcept {
        return m_config.ScreenWidth;
    }

    uint32_t FramebufferInterface::GetScreenHeight() const noexcept {
        return m_config.ScreenHeight;
    }
}  // namespace FunnyOS::HW