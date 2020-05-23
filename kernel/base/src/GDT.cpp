#include <FunnyOS/Kernel/GDT.hpp>

namespace FunnyOS::Kernel {

    namespace {
        struct GDTR {
            uint16_t Limit;
            uint64_t BaseAddress;
        } F_DONT_ALIGN;

        uint64_t g_kernelGdt[3];
    }  // namespace

    uint64_t CreateGdtDescriptor(const GDTEntry& entry) {
        uint64_t descriptor = 0;
        descriptor |= 1ULL << 44;  // Descriptor type, code or data

        if (entry.IsPresent) {
            descriptor |= 1ULL << 47;
        }

        if (entry.Type == GDTEntryType::Data) {
            descriptor |= 1ULL << 41;  // Writable attribute
        } else if (entry.Type == GDTEntryType::Code) {
            descriptor |= 1ULL << 43;                                                          // Code descriptor
            descriptor |= static_cast<uint64_t>(entry.DescriptorPrivilegeLevel & 0b11) << 45;  // DPL

            if (entry.IsConforming) {
                descriptor |= 1ULL << 42;
            }

            if (entry.IsLongMode) {
                descriptor |= 1ULL << 53;
            }

            if (entry.Is32Bit) {
                descriptor |= 1ULL << 54;
            }
        }

        return descriptor;
    }

    void LoadGdt(const Memory::SizedBuffer<uint64_t>& buffer) {
        const GDTR gdtr = {static_cast<uint16_t>(buffer.Size * sizeof(uint64_t) - 1),
                           reinterpret_cast<uint64_t>(buffer.Data)};

#ifdef __GNUC__
        asm("lgdt %0" ::"m"(gdtr) : "memory");
#endif
    }

    void LoadKernelGdt() {
        g_kernelGdt[0]                       = 0;
        g_kernelGdt[GDT_SELECTOR_DATA]       = CreateGdtDescriptor({.Type = GDTEntryType::Data, .IsPresent = true});
        g_kernelGdt[GDT_SELECTOR_CODE_RING0] = CreateGdtDescriptor({.Type                     = GDTEntryType::Code,
                                                                    .IsPresent                = true,
                                                                    .DescriptorPrivilegeLevel = 0,
                                                                    .IsConforming             = true,
                                                                    .IsLongMode               = true,
                                                                    .Is32Bit                  = false});

        LoadGdt({g_kernelGdt, F_SIZEOF_BUFFER(g_kernelGdt)});
    }

    void LoadNewSegments(uint16_t codeSegment, uint16_t dataSegment) {
#ifdef __GNUC__
        asm volatile(
            // Setup new segment registers
            "mov %[data_segment], %%ds       \n"
            "mov %[data_segment], %%es       \n"
            "mov %[data_segment], %%fs       \n"
            "mov %[data_segment], %%gs       \n"
            "mov %[data_segment], %%ss       \n"

            "mov %%rsp, %%rdx                \n"  // Save RSP
            "pushq %[data_segment]           \n"  // Push stack segment
            "push %%rdx                      \n"  // Push saved RSP
            "pushfq                          \n"  // Push flags
            "pushq %[code_segment]           \n"  // Push code segment

            // Calculate absolute address of label '2'
            "lea (%%rip), %%rdx              \n"
            "1:                              \n"
            "add $(2f - 1b), %%rdx           \n"
            "push %%rdx                      \n"  // Push new RIP

            // Use iretq to reload the CS
            "iretq                           \n"
            "2:                              \n"
            :
            : [ code_segment ] "a"(static_cast<uint64_t>(codeSegment << 3)),
              [ data_segment ] "b"(static_cast<uint64_t>(dataSegment << 3))
            : "memory", "rdx");
#endif
    }

    void LoadNewSegmentsAndJump(uint16_t codeSegment, uint16_t dataSegment, void* location) {
#ifdef __GNUC__
        asm volatile(
            // Setup new segment registers
            "mov %[data_segment], %%ds       \n"
            "mov %[data_segment], %%es       \n"
            "mov %[data_segment], %%fs       \n"
            "mov %[data_segment], %%gs       \n"
            "mov %[data_segment], %%ss       \n"

            "mov %%rsp, %%rdx                \n"  // Save RSP
            "pushq %[data_segment]           \n"  // Push stack segment
            "push %%rdx                      \n"  // Push saved RSP
            "pushfq                          \n"  // Push flags
            "pushq %[code_segment]           \n"  // Push code segment
            "push %[jump_location]           \n"  // Push new RIP

            // Use iretq to reload the CS
            "iretq                           \n"
            "2:                              \n"
            :
            : [ code_segment ] "a"(static_cast<uint64_t>(codeSegment << 3)),
              [ data_segment ] "b"(static_cast<uint64_t>(dataSegment << 3)), [ jump_location ] "D"(location)
            : "memory");
#endif
        F_NO_RETURN;
    }

}  // namespace FunnyOS::Kernel