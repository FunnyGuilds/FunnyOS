#ifndef F_64
#include <FunnyOS/Stdlib/IntegerTypes.hpp>

namespace {
    struct QWordParts {
        uint32_t Low;
        uint32_t High;
    };

    union Wrapper {
        uint64_t Full;
        QWordParts Parts;
    };
}  // namespace

using namespace FunnyOS::Stdlib;

extern "C" {

/**
 * Shifts [number] left by [shift] bits.
 */
uint64_t __ashldi3(uint64_t number, unsigned int shift) {
    if (shift == 0) {
        // Nothing to do
        return number;
    }

    // Prepare wrapper
    Wrapper output{.Full = number};

    if (shift >= 32) {
        // Shift discards lower bits.
        output.Parts.High = output.Parts.Low << (shift - 32);
        output.Parts.Low = 0;
    } else {
        // Regular shift
        output.Parts.High <<= shift;
        output.Parts.High |= output.Parts.Low >> (32 - shift);
        output.Parts.Low <<= shift;
    }

    return output.Full;
}

/**
 * Shifts [number] right by [shift] bits.
 */
uint64_t __lshrdi3(uint64_t number, unsigned int shift) {
    if (shift == 0) {
        // Nothing to do
        return number;
    }

    Wrapper output{.Full = number};

    if (shift >= 32) {
        // Shift discards higher bits.
        output.Parts.Low = output.Parts.Low >> (shift - 32);
        output.Parts.High = 0;
    } else {
        // Regular shift
        output.Parts.Low >>= shift;
        output.Parts.Low |= output.Parts.High << (32 - shift);
        output.Parts.High >>= shift;
    }

    return output.Full;
}

namespace {
    void DivideUnsigned(uint64_t dividend, uint64_t divisor, uint64_t& quotient, uint64_t& reminder) {
        if (divisor == 0) {
            // Division by 0
            quotient = 1 / static_cast<uint32_t>(divisor);
            return;
        }

        // do 32-bit divide if possible
        if (dividend <= NumeralTraits::Info<uint32_t>::MaximumValue) {
            if (divisor > dividend) {
                quotient = 0;
                reminder = dividend;
            } else {
                quotient = static_cast<uint32_t>(dividend) / static_cast<uint32_t>(divisor);
                reminder = static_cast<uint32_t>(dividend) % static_cast<uint32_t>(divisor);
            }

            return;
        }

        // Dividing bit by bit, the slowest case
        quotient = 0;

        while (divisor <= dividend) {
            const uint64_t maxDivisor = __lshrdi3(dividend, 1);
            uint64_t currentDivisor = divisor;

            int shiftCount = 0;
            while (currentDivisor <= maxDivisor) {
                shiftCount++;
                currentDivisor = currentDivisor + currentDivisor;
            }

            quotient |= __ashldi3(1, shiftCount);
            dividend -= currentDivisor;
        }

        reminder = dividend;
    }

    void DivideSigned(int64_t dividend, int64_t divisor, int64_t& quotient, int64_t& reminder) {
        // Sign calculations
        const bool dividendPositive = dividend >= 0;
        const bool divisorPositive = divisor >= 0;
        const bool quotientPositive = dividendPositive ^ divisorPositive;
        const bool reminderPositive = dividendPositive;

        // Unsigned division
        uint64_t quotient64;
        uint64_t reminder64;

        DivideUnsigned(static_cast<uint64_t>(dividend >= 0 ? dividend : -dividend),
                       static_cast<uint64_t>(divisor >= 0 ? divisor : -divisor), quotient64, reminder64);

        // Sign adjustments
        quotient = static_cast<int64_t>(quotientPositive ? quotient64 : -quotient64);
        reminder = static_cast<int64_t>(reminderPositive ? reminder64 : -reminder64);
    }
}  // namespace

uint64_t __udivdi3(uint64_t dividend, uint64_t divisor) {
    uint64_t quotient;
    uint64_t reminder;
    DivideUnsigned(dividend, divisor, quotient, reminder);
    return quotient;
}

uint64_t __umoddi3(uint64_t dividend, uint64_t divisor) {
    uint64_t quotient;
    uint64_t reminder;
    DivideUnsigned(dividend, divisor, quotient, reminder);
    return reminder;
}

int64_t __divdi3(int64_t dividend, int64_t divisor) {
    int64_t quotient;
    int64_t reminder;
    DivideSigned(dividend, divisor, quotient, reminder);
    return quotient;
}

int64_t __moddi3(int64_t dividend, int64_t divisor) {
    int64_t quotient;
    int64_t reminder;
    DivideSigned(dividend, divisor, quotient, reminder);
    return quotient;
}

uint64_t __udivmoddi4(uint64_t dividend, uint64_t divisor, uint64_t* remainder) {
    uint64_t quotient;
    DivideUnsigned(dividend, divisor, quotient, *remainder);
    return quotient;
}

int64_t __divmoddi4(int64_t dividend, int64_t divisor, int64_t* remainder) {
    int64_t quotient;
    DivideSigned(dividend, divisor, quotient, *remainder);
    return quotient;
}
}
#endif
