#include <FunnyOS/Hardware/BIOS.hpp>

#include <FunnyOS/Stdlib/Compiler.hpp>
#include <FunnyOS/Stdlib/Vector.hpp>
#include <FunnyOS/Hardware/Interrupts.hpp>

struct BiosRegisters {
    uint32_t EAX, ECX, EDX, EBX, EDI, ESI, EBP, EFLAGS, DS, ES, FS, GS;
};

struct BiosState {
    // BiosRegisters
    BiosRegisters Registers;

    // Selectors
    uint16_t Selector64BitCode, Selector64BitData, Selector16BitCode, Selector16BitData;

    // Interrupt number
    uint8_t InterruptNumber;
};

extern volatile BiosState g_biosState;  // bios.asm
extern "C" void do_call_bios();

namespace FunnyOS::HW::BIOS {
    using namespace Stdlib::String;

    uint16_t GetRealModeSegment(void* address) {
        return (reinterpret_cast<uintptr_t>(address) & 0xFFFF0) >> 4;
    }

    uint16_t GetRealModeOffset(void* address) {
        return (reinterpret_cast<uintptr_t>(address) & 0x0000F) >> 0;
    }

    void SetupCallBiosInterface(
        uint16_t selector64BitCode, uint16_t selector64BitData, uint16_t selector16BitCode,
        uint16_t selector16BitData) {
        g_biosState.Selector64BitCode = selector64BitCode << 3;
        g_biosState.Selector64BitData = selector64BitData << 3;
        g_biosState.Selector16BitCode = selector16BitCode << 3;
        g_biosState.Selector16BitData = selector16BitData << 3;
    }

    namespace {
        /**
         * Represents a way to access a value or a part of value of a 32-bit register.
         */
        struct RegisterAlias {
            const char* Name;
            volatile uint32_t& Ref;
            uint32_t RegisterMask;
            uint32_t ValueMask;
            size_t Shift;
        };

        RegisterAlias g_registerAliases[] = {
            // EAX
            {"eax", g_biosState.Registers.EAX, 0xFFFFFFFF, 0xFFFFFFFF, 0},
            {"ax", g_biosState.Registers.EAX, 0x0000FFFF, 0x0000FFFF, 0},
            {"ah", g_biosState.Registers.EAX, 0x0000FF00, 0x000000FF, 8},
            {"al", g_biosState.Registers.EAX, 0x000000FF, 0x000000FF, 0},

            // ECX
            {"ecx", g_biosState.Registers.ECX, 0xFFFFFFFF, 0xFFFFFFFF, 0},
            {"cx", g_biosState.Registers.ECX, 0x0000FFFF, 0x0000FFFF, 0},
            {"ch", g_biosState.Registers.ECX, 0x0000FF00, 0x000000FF, 8},
            {"cl", g_biosState.Registers.ECX, 0x000000FF, 0x000000FF, 0},

            // EDX
            {"edx", g_biosState.Registers.EDX, 0xFFFFFFFF, 0xFFFFFFFF, 0},
            {"dx", g_biosState.Registers.EDX, 0x0000FFFF, 0x0000FFFF, 0},
            {"dh", g_biosState.Registers.EDX, 0x0000FF00, 0x000000FF, 8},
            {"dl", g_biosState.Registers.EDX, 0x000000FF, 0x000000FF, 0},

            // EBX
            {"ebx", g_biosState.Registers.EBX, 0xFFFFFFFF, 0xFFFFFFFF, 0},
            {"bx", g_biosState.Registers.EBX, 0x0000FFFF, 0x0000FFFF, 0},
            {"bh", g_biosState.Registers.EBX, 0x0000FF00, 0x000000FF, 8},
            {"bl", g_biosState.Registers.EBX, 0x000000FF, 0x000000FF, 0},

            // EDI
            {"edi", g_biosState.Registers.EDI, 0xFFFFFFFF, 0xFFFFFFFF, 0},
            {"di", g_biosState.Registers.EDI, 0x0000FFFF, 0x0000FFFF, 0},
            {"doh", g_biosState.Registers.EDI, 0x0000FF00, 0x000000FF, 8},
            {"dol", g_biosState.Registers.EDI, 0x000000FF, 0x000000FF, 0},

            // ESI
            {"esi", g_biosState.Registers.ESI, 0xFFFFFFFF, 0xFFFFFFFF, 0},
            {"si", g_biosState.Registers.ESI, 0x0000FFFF, 0x0000FFFF, 0},
            {"soh", g_biosState.Registers.ESI, 0x0000FF00, 0x000000FF, 8},
            {"sol", g_biosState.Registers.ESI, 0x000000FF, 0x000000FF, 0},

            // EBP
            {"ebp", g_biosState.Registers.EBP, 0xFFFFFFFF, 0xFFFFFFFF, 0},
            {"bp", g_biosState.Registers.EBP, 0x0000FFFF, 0x0000FFFF, 0},

            // EFLAGS
            {"eflags", g_biosState.Registers.EFLAGS, 0xFFFFFFFF, 0xFFFFFFFF, 0},
            {"flags", g_biosState.Registers.EFLAGS, 0x0000FFFF, 0x0000FFFF, 0},

            // DS, ES, FS, GS
            {"ds", g_biosState.Registers.DS, 0x0000FFFF, 0x0000FFFF, 0},
            {"es", g_biosState.Registers.ES, 0x0000FFFF, 0x0000FFFF, 0},
            {"fs", g_biosState.Registers.FS, 0x0000FFFF, 0x0000FFFF, 0},
            {"gs", g_biosState.Registers.GS, 0x0000FFFF, 0x0000FFFF, 0},
        };

        class IRegisterAccessor {
            INTERFACE(IRegisterAccessor);

            virtual void SetValue(uint32_t value) = 0;

            virtual uint32_t GetValue() const = 0;

            virtual bool IsInputLong() const = 0;

            virtual bool IsOutput() const = 0;
        };

        class StandardRegisterAccessor : public IRegisterAccessor {
           public:
            StandardRegisterAccessor(RegisterAlias& description, bool output)
                : m_description{description}, m_output{output} {}

            void SetValue(uint32_t value) override {
                m_description.Ref = m_description.Ref & ~m_description.RegisterMask;
                m_description.Ref = m_description.Ref | ((value & m_description.ValueMask) << m_description.Shift);
            }

            virtual uint32_t GetValue() const override {
                return ((m_description.Ref & m_description.RegisterMask) >> m_description.Shift) &
                       m_description.ValueMask;
            }

            bool IsInputLong() const override {
                return false;
            }

            bool IsOutput() const override {
                return m_output;
            }

           private:
            RegisterAlias& m_description;
            const bool m_output;
        };

        class SegmentOffsetRegisterPairAccessor : public IRegisterAccessor {
           public:
            SegmentOffsetRegisterPairAccessor(
                Stdlib::Owner<IRegisterAccessor> segment, Stdlib::Owner<IRegisterAccessor> offset)
                : m_segment{Stdlib::Move(segment)}, m_offset{Stdlib::Move(offset)} {
                F_ASSERT(m_segment->IsOutput() == m_offset->IsOutput(), "segment/offset accessor mismatch");
            }

            void SetValue(uint32_t value) override {
                m_segment->SetValue(GetRealModeSegment(reinterpret_cast<void*>(value)));
                m_offset->SetValue(GetRealModeOffset(reinterpret_cast<void*>(value)));
            }

            uint32_t GetValue() const override {
                return (m_segment->GetValue()) << 4 | m_offset->GetValue();
            }

            bool IsInputLong() const override {
                return true;
            }

            bool IsOutput() const override {
                return m_segment->IsOutput();
            }

           private:
            Stdlib::Owner<IRegisterAccessor> m_segment;
            Stdlib::Owner<IRegisterAccessor> m_offset;
        };

        RegisterAlias* ParseRegisterAlias(const char* string) {
            for (size_t i = 0; i < F_SIZEOF_BUFFER(g_registerAliases); ++i) {
                if (Compare(g_registerAliases[i].Name, string) != 0) {
                    continue;
                }

                return &g_registerAliases[i];
            }

            return nullptr;
        }

        Stdlib::Owner<IRegisterAccessor> ParseRegisterAccessor(const char* string) {
            bool isOutput = *string == '=';

            if (isOutput) {
                string++;
            }

            SmartStringBuffer stringCopy = AllocateSmartCopy(string);
            char* tokens                 = stringCopy.Data;

            char* firstValue = NextToken(&tokens, ":");

            if (tokens == nullptr) {
                // No ':', standard accessor name
                RegisterAlias* description = ParseRegisterAlias(string);

                if (!description) {
                    return {};
                }

                return Stdlib::MakeOwnerBase<IRegisterAccessor, StandardRegisterAccessor>(*description, isOutput);
            }

            // segment:offset register pair
            char* secondValue = NextToken(&tokens, ":");

            RegisterAlias* segment = ParseRegisterAlias(firstValue);
            RegisterAlias* offset  = ParseRegisterAlias(secondValue);

            if (!segment || !offset) {
                return {};
            }

            return Stdlib::MakeOwnerBase<IRegisterAccessor, SegmentOffsetRegisterPairAccessor>(
                Stdlib::MakeOwnerBase<IRegisterAccessor, StandardRegisterAccessor>(*segment, isOutput),
                Stdlib::MakeOwnerBase<IRegisterAccessor, StandardRegisterAccessor>(*offset, isOutput));
        }

    }  // namespace

    bool CallBios(uint8_t interrupt, const char* format, ...) {
        va_list inputArguments;
        va_list outputArguments;

        va_start(inputArguments, format);
        va_copy(outputArguments, inputArguments);

        // Parse format
        Stdlib::Vector<Stdlib::Owner<IRegisterAccessor>> accessors;
        accessors.EnsureCapacity(Count(format, ",") + 1);

        SmartStringBuffer formatCopy = AllocateSmartCopy(format);
        char* tokens                 = formatCopy.Data;

        while (tokens != nullptr) {
            char* token = NextToken(&tokens, ",");
            Trim(&token);
            ToLowercase(token);

            Stdlib::Owner<IRegisterAccessor> alias = ParseRegisterAccessor(token);
            F_ASSERT(alias.Get() != nullptr, "invalid alias");

            accessors.AppendInPlace(Stdlib::Move(alias));
        }

        // Clear default register values
        Stdlib::Memory::ZeroMemory(g_biosState.Registers);

        // Set input values
        for (auto& accessor : accessors) {
            if (accessor->IsOutput()) {
                va_arg(inputArguments, void*);
                continue;
            }

            uint32_t value;

            if (accessor->IsInputLong()) {
                uintptr_t ptr = reinterpret_cast<uintptr_t>(va_arg(inputArguments, void*));

                F_ASSERT(ptr <= Stdlib::NumeralTraits::Info<uint32_t>::MaximumValue, "BIOS ptr larger than 32 bit");

                value = static_cast<uint32_t>(ptr);
            } else {
                value = va_arg(inputArguments, uint32_t);
            }

            accessor->SetValue(value);
        }
        va_end(inputArguments);

        // non-register parameters
        g_biosState.InterruptNumber = interrupt;
        F_MEMORY_FENCE;
        do_call_bios();
        F_MEMORY_FENCE;

        // Output arguments
        for (auto& accessor : accessors) {
            if (!accessor->IsOutput()) {
                if (accessor->IsInputLong()) {
                    va_arg(outputArguments, void*);
                } else {
                    va_arg(outputArguments, uint32_t);
                }

                continue;
            }

            *va_arg(outputArguments, uint32_t*) = accessor->GetValue();
        }

        va_end(outputArguments);

        return (g_biosState.Registers.EFLAGS & static_cast<uint32_t>(HW::CPU::Flags::CarryFlag)) == 0;
    }

}  // namespace FunnyOS::HW::BIOS
