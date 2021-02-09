#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GDT_HPP
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_GDT_HPP

#include<FunnyOS/Stdlib/IntegerTypes.hpp>
#include<FunnyOS/Stdlib/Memory.hpp>

namespace FunnyOS::HW {
    using namespace Stdlib;

    /**
     * Type of a GDT entry.
     */
    enum class GDTEntryType : uint8_t {
        /**
         * Indicates a data segment
         */
        Data = 0,

        /**
         * Indicates a code segment
         */
        Code = 1
    };

    /**
     * GDT descriptor info.
     */
    struct GDTEntry {

        /**
         * Entry's base address.
         */
        uint64_t BaseAddress;

        /**
         * Entry's limit.
         */
        uint64_t Limit;

        /**
         * Type of the entry. Code or Data.
         */
        GDTEntryType Type = GDTEntryType::Data;

        /**
         * Granularity bit, if set entry's limit is scaled by 4 Kbytes
         */
        bool GranularityBit = false;

        /**
         * Whether or not the entry is present.
         */
        bool IsPresent = false;

        /**
         * The privilege level of this descriptor.
         *
         * Only for Code segments.
         */
        uint8_t DescriptorPrivilegeLevel = 0;

        /**
         * If true this segment may be called by lower privilege levels.
         *
         * Only for Code segments.
         */
        bool IsConforming = false;

        /**
         * Is this a long mode segment?
         *
         * Only for Code segments.
         */
        bool IsLongMode = false;

        /**
         * Is this a 32-bit segment?
         * If false segment is 16-bit or 64-bit if IsLongMode is true.
         *
         * Only for Code segments.
         */
        bool Is32Bit = false;
    };

    using gdt_descriptor_t = uint64_t;

    /**
     * Creates a GDT descriptor from the given [entry]
     */
    gdt_descriptor_t CreateGdtDescriptor(const GDTEntry& entry);

    /**
     * Loads the given [gdt].
     */
    void LoadGdt(const Memory::SizedBuffer<gdt_descriptor_t>& gdt);

    /**
     * Loads the segment values with the given segments.
     *
     * @param codeSegment code segment to jump to
     * @param dataSegment data segment to use
     */
    void LoadNewSegments(uint16_t codeSegment, uint16_t dataSegment);

    /**
     * Loads the segment values with the given segments and immediately jumps to [location].
     *
     * @param codeSegment code segment to jump to
     * @param dataSegment data segment to use
     * @param location location where to jump after reloading the segments.
     */
    [[noreturn]] void LoadNewSegmentsAndJump(uint16_t codeSegment, uint16_t dataSegment, void* location);

}  // namespace FunnyOS::HW

#endif  // FUNNYOS_KERNEL_BASE_SRC_HW_GDT_HPP
