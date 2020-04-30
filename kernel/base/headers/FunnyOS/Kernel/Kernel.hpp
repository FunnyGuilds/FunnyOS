#ifndef FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_KERNEL_HPP
#define FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_KERNEL_HPP

#include <FunnyOS/Bootparams/Parameters.hpp>

namespace FunnyOS::Kernel {

    class Kernel64 {
       public:
        static Kernel64& Get();

        void Initialize(Bootparams::Parameters& parameters);

        [[nodiscard]] const Bootparams::Parameters& GetParameters() const;

       private:
        Kernel64();

       private:
        Bootparams::Parameters m_parameters{};
    };

}  // namespace FunnyOS::Kernel

#endif  // FUNNYOS_KERNEL_BASE_HEADERS_FUNNYOS_KERNEL_KERNEL_HPP
