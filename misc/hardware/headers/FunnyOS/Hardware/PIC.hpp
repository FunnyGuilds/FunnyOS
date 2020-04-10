#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_PIC_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_PIC_HPP

#include "Interrupts.hpp"

namespace FunnyOS::HW::PIC {

    bool IsPICInterrupt(InterruptType type);

    bool IsMasterPICInterrupt(InterruptType type);

    void Remap();

    void SendEndOfInterrupt(bool master);
}


#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_PIC_HPP
