#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_HPP
#error "Include CPU.hpp instead"
#endif

#ifndef FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_TCC
#define FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_TCC

namespace FunnyOS::HW::CPU {
    namespace _CPUID {
        template <typename T>
        inline bool AppendIfTest(T value, T mask, const char* string, Stdlib::String::StringBuffer& buffer) {
            if ((value & mask) == 0) {
                return true;
            }

            return Stdlib::String::Append(buffer, string);
        }

        inline uint32_t FetchCpuidMaxFeature() {
            uint32_t ignored = 0;
            uint32_t eax = 0;
            CallCpuid(eax, ignored, ignored, ignored);
            return eax;
        }

        inline uint32_t FetchCpuidMaxExtendedFeature() {
            uint32_t ignored = 0;
            uint32_t eax = 0x80000000;
            CallCpuid(eax, ignored, ignored, ignored);
            return eax;
        }

        inline uint64_t FetchFeatureBits() {
            uint32_t eax = 1;
            uint32_t ecx = 0;
            uint32_t edx = 0;
            uint32_t ebx = 0;

            CallCpuid(eax, ecx, edx, ebx);
            return static_cast<uint64_t>(edx) | (static_cast<uint64_t>(ecx) << 32ULL);
        }

        inline uint64_t FetchExtendedFeatureBits() {
            uint32_t eax = 0x80000001;
            uint32_t ecx = 0;
            uint32_t edx = 0;
            uint32_t ebx = 0;

            if (GetCpuidMaxExtendedFeature() < eax) {
                return 0;
            }

            CallCpuid(eax, ecx, edx, ebx);
            return static_cast<uint64_t>(edx) | (static_cast<uint64_t>(ecx) << 32ULL);
        }
    }  // namespace _CPUID

    inline uint32_t GetCpuidMaxFeature() {
        static uint32_t s_cache = _CPUID::FetchCpuidMaxFeature();
        return s_cache;
    }

    inline uint32_t GetCpuidMaxExtendedFeature() {
        static uint32_t s_cache = _CPUID::FetchCpuidMaxExtendedFeature();
        return s_cache;
    }

    inline void GetVendorId(Stdlib::String::StringBuffer& buffer) {
        F_ASSERT(buffer.Size >= 13, "vendor id buffer size < 13");

        uint32_t num = 0;
        auto* out1 = reinterpret_cast<uint32_t*>(buffer.Data);
        auto* out2 = reinterpret_cast<uint32_t*>(buffer.Data + 4);
        auto* out3 = reinterpret_cast<uint32_t*>(buffer.Data + 8);
        CallCpuid(num, *out3, *out2, *out1);
        buffer.Data[12] = 0;
    }

    /**
     * Get extended feature bits.
     *
     * @return extended feature bits
     */
    inline uint64_t GetFeatureBits() {
        static uint64_t s_cache = _CPUID::FetchFeatureBits();
        return s_cache;
    }

    inline uint64_t GetExtendedFeatureBits() {
        static uint64_t s_cache = _CPUID::FetchExtendedFeatureBits();
        return s_cache;
    }

    inline bool GetBrandString(Stdlib::String::StringBuffer& buffer) {
        if (buffer.Size < 48 || GetCpuidMaxExtendedFeature() < 0x80000004) {
            return false;
        }

        uint32_t strBuf[] = {
            0x80000002, 0, 0, 0, 0x80000003, 0, 0, 0, 0x80000004, 0, 0, 0,
        };

        CallCpuid(strBuf[0], strBuf[2], strBuf[3], strBuf[1]);
        CallCpuid(strBuf[4], strBuf[6], strBuf[7], strBuf[5]);
        CallCpuid(strBuf[8], strBuf[10], strBuf[11], strBuf[9]);
        Stdlib::Memory::Copy(static_cast<void*>(buffer.Data), static_cast<void*>(strBuf), 48);
        return true;
    }

#define DECODE_HELPER(feature)                                                                            \
    if (!_CPUID::AppendIfTest<uint64_t>(                                                                  \
            features, static_cast<uint64_t>(CPUIDFeatures::feature), F_TO_STRING(feature) " ", buffer)) { \
        return false;                                                                                     \
    }

    inline bool DecodeFeatureBits(uint64_t features, Stdlib::String::StringBuffer& buffer) {
        DECODE_HELPER(FPU)
        DECODE_HELPER(VME)
        DECODE_HELPER(DE)
        DECODE_HELPER(PSE)
        DECODE_HELPER(TSC)
        DECODE_HELPER(MSR)
        DECODE_HELPER(PAE)
        DECODE_HELPER(MCE)
        DECODE_HELPER(CX8)
        DECODE_HELPER(APIC)
        DECODE_HELPER(SEP)
        DECODE_HELPER(MTRR)
        DECODE_HELPER(PGE)
        DECODE_HELPER(MCA)
        DECODE_HELPER(CMOV)
        DECODE_HELPER(PAT)
        DECODE_HELPER(PSE36)
        DECODE_HELPER(PSN)
        DECODE_HELPER(CLFSH)
        DECODE_HELPER(DS)
        DECODE_HELPER(ACPI)
        DECODE_HELPER(MMX)
        DECODE_HELPER(FXSR)
        DECODE_HELPER(SSE)
        DECODE_HELPER(SSE2)
        DECODE_HELPER(SS)
        DECODE_HELPER(HTT)
        DECODE_HELPER(TM)
        DECODE_HELPER(IA64)
        DECODE_HELPER(PBE)
        DECODE_HELPER(SSE3)
        DECODE_HELPER(PCLMULQDQ)
        DECODE_HELPER(DTES64)
        DECODE_HELPER(MONITOR)
        DECODE_HELPER(DSCPL)
        DECODE_HELPER(VMX)
        DECODE_HELPER(SMX)
        DECODE_HELPER(EST)
        DECODE_HELPER(TM2)
        DECODE_HELPER(SSSE3)
        DECODE_HELPER(CNXTID)
        DECODE_HELPER(SDBG)
        DECODE_HELPER(FMA)
        DECODE_HELPER(CX16)
        DECODE_HELPER(XTPR)
        DECODE_HELPER(PDCM)
        DECODE_HELPER(PCID)
        DECODE_HELPER(DCA)
        DECODE_HELPER(SSE41)
        DECODE_HELPER(SSE42)
        DECODE_HELPER(X2APIC)
        DECODE_HELPER(MOVBE)
        DECODE_HELPER(POPCNT)
        DECODE_HELPER(TSCDEADLINE)
        DECODE_HELPER(AES)
        DECODE_HELPER(XSAVE)
        DECODE_HELPER(OSXSAVE)
        DECODE_HELPER(AVX)
        DECODE_HELPER(F16C)
        DECODE_HELPER(RDRND)
        DECODE_HELPER(HYPERVISOR)
        return true;
    }
#undef DECODE_HELPER

#define DECODE_HELPER(feature)                                                                                    \
    if (!_CPUID::AppendIfTest<uint64_t>(                                                                          \
            features, static_cast<uint64_t>(CPUIDExtendedFeatures::feature), F_TO_STRING(feature) " ", buffer)) { \
        return false;                                                                                             \
    }

    inline bool DecodeExtendedFeatureBits(uint64_t features, Stdlib::String::StringBuffer& buffer) {
        DECODE_HELPER(FPU)
        DECODE_HELPER(VME)
        DECODE_HELPER(DE)
        DECODE_HELPER(PSE)
        DECODE_HELPER(TSC)
        DECODE_HELPER(MSR)
        DECODE_HELPER(PAE)
        DECODE_HELPER(MCE)
        DECODE_HELPER(CX8)
        DECODE_HELPER(APIC)
        DECODE_HELPER(SYSCALL)
        DECODE_HELPER(MTRR)
        DECODE_HELPER(PGE)
        DECODE_HELPER(MCA)
        DECODE_HELPER(CMOV)
        DECODE_HELPER(PAT)
        DECODE_HELPER(PSE36)
        DECODE_HELPER(MP)
        DECODE_HELPER(NX)
        DECODE_HELPER(MMXEXT)
        DECODE_HELPER(MMX)
        DECODE_HELPER(FXSR)
        DECODE_HELPER(FXSR_OPT)
        DECODE_HELPER(PDPE1GB)
        DECODE_HELPER(RDTSCP)
        DECODE_HELPER(LM)
        DECODE_HELPER(EXT_3DNOW)
        DECODE_HELPER(HAS_3DNOW)
        DECODE_HELPER(LAHF_LM)
        DECODE_HELPER(CMP_LEGACY)
        DECODE_HELPER(SVM)
        DECODE_HELPER(EXTAPIC)
        DECODE_HELPER(CR8_LEGACY)
        DECODE_HELPER(ABM)
        DECODE_HELPER(SSE4A)
        DECODE_HELPER(MISALIGN_SSE)
        DECODE_HELPER(HAS_3DNOWPREFETCH)
        DECODE_HELPER(OSVW)
        DECODE_HELPER(IBS)
        DECODE_HELPER(XOP)
        DECODE_HELPER(SKINIT)
        DECODE_HELPER(WDT)
        DECODE_HELPER(LWP)
        DECODE_HELPER(FMA4)
        DECODE_HELPER(TCE)
        DECODE_HELPER(NODEID_MSR)
        DECODE_HELPER(TBM)
        DECODE_HELPER(TOPOEXT)
        DECODE_HELPER(PERFCTR_CORE)
        DECODE_HELPER(PERFCTR_NB)
        DECODE_HELPER(DBX)
        DECODE_HELPER(PERFTSC)
        DECODE_HELPER(PCX_L2I)
        return true;
    }
#undef DECODE_HELPER
}  // namespace FunnyOS::HW::CPU

#endif  // FUNNYOS_MISC_HARDWARE_HEADERS_FUNNYOS_HARDWARE_CPU_TCC
